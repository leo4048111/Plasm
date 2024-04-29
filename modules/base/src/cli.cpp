#include "cli.hpp"

#include <string>
#include <optional>
#include <set>
#include <map>
#include <chrono>
#include <sstream>

#include "logger.hpp"
#include "noopgenerator.hpp"
#include "cpnidegenerator.hpp"
#include "generator.hpp"
#include "visualizer.hpp"
#include "simulator.hpp"
#include "report.hpp"
#include "csv.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>

#include <libsolutil/Common.h>
#include <libsolutil/CommonData.h>
#include <libsolutil/CommonIO.h>
#include <libsolutil/JSON.h>

#include <libsolidity/interface/CompilerStack.h>
#include <libsolidity/ast/ASTJsonExporter.h>

_START_PSM_NM_

void CLI::Run(int argc, char const *const *argv)
{
    // Pipeline
    ParseArgs(argc, argv);
    ProcessArgs();
    ReadInputFiles();
    ProcessInput();
}

void CLI::ParseArgs(int argc, char const *const *argv)
{
    po::options_description allOptions = GetOptionsDescription();
    po::positional_options_description filesPositions = GetPositionalOptionsDescription();

    options_.Clear();
    args_ = {};

    // parse the compiler arguments
    try
    {
        po::command_line_parser cmdLineParser(argc, argv);
        cmdLineParser.style(po::command_line_style::default_style & (~po::command_line_style::allow_guessing));
        cmdLineParser.options(allOptions).positional(filesPositions);
        po::store(cmdLineParser.run(), args_);
    }
    catch (po::error const &exception)
    {
        LOGE(exception.what());
        PSM_BAIL();
    }

    po::notify(args_);
}

void CLI::ProcessArgs()
{
    // process mode
    if (args_.count(g_strHelp) > 0)
        options_.input.mode = CLIMode::Help;
    else if (args_.count(g_strLicense) > 0)
        options_.input.mode = CLIMode::License;
    else if (args_.count(g_strVersion) > 0)
        options_.input.mode = CLIMode::Version;
    else if (args_.count(g_strSimulate) > 0)
        options_.input.mode = CLIMode::Simulate;
    else
        options_.input.mode = CLIMode::NetGen;

    // process inputs
    options_.input.ignoreMissingFiles = (args_.count(g_strIgnoreMissingFiles) > 0);
    if (args_.count(g_strBasePath))
        options_.input.basePath = args_[g_strBasePath].as<std::string>();

    if (args_.count(g_strIncludePath) > 0)
    {
        if (options_.input.basePath.empty())
            LOGW("%s%s%s", "--", g_strIncludePath.c_str(), " option requires a non-empty base path.");

        for (std::string const &includePath : args_[g_strIncludePath].as<std::vector<std::string>>())
        {
            if (includePath.empty())
                LOGW("%s%s%s", "Empty values are not allowed in --", g_strIncludePath.c_str(), ".");

            options_.input.includePaths.push_back(includePath);
        }
    }

    if (args_.count(g_strInputFile))
    {
        for (::std::string const &positionalArg : args_[g_strInputFile].as<std::vector<std::string>>())
        {
            options_.input.paths.insert(positionalArg);
        }
    }

    // outputs
    if (args_.count(g_strOutputDir))
        options_.output.dir = args_.at(g_strOutputDir).as<::std::string>();

    if (args_.count(g_strEVMVersion))
    {
        ::std::string versionOptionStr = args_[g_strEVMVersion].as<std::string>();
        ::std::optional<solidity::langutil::EVMVersion> versionOption = solidity::langutil::EVMVersion::fromString(versionOptionStr);
        if (!versionOption)
            LOGW("%s%s%s%s", "Invalid option for --", g_strEVMVersion.c_str(), ": ", versionOptionStr.c_str());
        options_.output.evmVersion = *versionOption;
    }
}

void CLI::ReadInputFiles()
{
    static ::std::set<CLIMode> const noInputFiles{
        CLIMode::Help,
        CLIMode::License,
        CLIMode::Version};

    // check if cli should have inputs
    if (noInputFiles.count(options_.input.mode) == 1)
        return;

    fileReader_.setBasePath(options_.input.basePath);

    if (fileReader_.basePath() != "")
    {
        if (!boost::filesystem::exists(fileReader_.basePath()))
            LOGW("%s%s%s", "Base path does not exist: \"", fileReader_.basePath().string().c_str(), '"');

        if (!boost::filesystem::is_directory(fileReader_.basePath()))
            LOGW("%s%s%s", "Base path is not a directory: \"", fileReader_.basePath().string().c_str(), '"');
    }

    for (boost::filesystem::path const &includePath : options_.input.includePaths)
        fileReader_.addIncludePath(includePath);

    ::std::map<std::string, std::set<boost::filesystem::path>> collisions =
        fileReader_.detectSourceUnitNameCollisions(options_.input.paths);

    if (!collisions.empty())
    {
        std::string message =
            "Source unit name collision detected. "
            "The specified values of base path and/or include paths would result in multiple "
            "input files being assigned the same source unit name:\n";
        LOGE(message.c_str());
        PSM_BAIL();
    }

    for (boost::filesystem::path const &infile : options_.input.paths)
    {
        if (!boost::filesystem::exists(infile))
        {
            if (!options_.input.ignoreMissingFiles)
            {
                LOGE("\"%s\" is not found.", infile.string().c_str());
                PSM_BAIL();
            }
            else
                LOGW("\"%s\" is not found. Skipping.", infile.string().c_str());

            continue;
        }

        // check if path is directory or regular file
        std::vector<boost::filesystem::path> expandedPaths;
        if (boost::filesystem::is_directory(infile))
        {
            boost::filesystem::directory_iterator end_itr;
            for (boost::filesystem::directory_iterator itr(infile); itr != end_itr; ++itr)
            {
                if (boost::filesystem::is_regular_file(itr->path()))
                {
                    // check file extention
                    // TODO: supports regular expression
                    if (itr->path().extension() != ".sol")
                    {
                        continue;
                    }
                    expandedPaths.push_back(itr->path());
                    LOGI("Found file: %s", itr->path().string().c_str());
                }
            }
        }
        else if (boost::filesystem::is_regular_file(infile))
        {
            expandedPaths.push_back(infile);
        }
        else
        {
            if (!options_.input.ignoreMissingFiles)
            {
                LOGE("%s%s%s", '"', infile.string().c_str(), "\" is not a valid regular file or directory.");
                PSM_BAIL();
            }
            else
                LOGW("%s is not a valid regular file or directory. Skipping.", infile.string().c_str());
            continue;
        }

        for (auto const &infile : expandedPaths)
        {
            ::std::string fileContent = solidity::util::readFileAsString(infile);
            fileReader_.addOrUpdateFile(infile, ::std::move(fileContent));
            fileReader_.allowDirectory(boost::filesystem::canonical(infile).remove_filename());
        }
    }

    if (fileReader_.sourceUnits().empty())
    {
        LOGE("All specified input files either do not exist or are not regular files.");
        PSM_BAIL();
    }

    if (!boost::filesystem::is_directory(options_.input.resultPath))
        boost::filesystem::create_directories(options_.input.resultPath);
}

void CLI::ProcessInput()
{
    switch (options_.input.mode)
    {
    case CLIMode::Help:
        PrintHelp();
        break;
    case CLIMode::License:
        PrintLicense();
        break;
    case CLIMode::Version:
        break;
    case CLIMode::Simulate:
        CompileAndGenerate(true);
        break;
    case CLIMode::NetGen:
        CompileAndGenerate();
        break;
    }
}

void CLI::PrintHelp() const
{
    ::std::ostringstream oss;
    oss << GetOptionsDescription();

    LOGI(oss.str().c_str());
}

void CLI::PrintLicense() const
{
    const char *wtfplLicenseText =
        "\n"
        "            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE\n"
        "                    Version 2, December 2004\n"
        "\n"
        " Copyright (C) [2024] [Z3r0M3r0ry]\n"
        "\n"
        " Everyone is permitted to copy and distribute verbatim or modified\n"
        " copies of this license document, and changing it is allowed as long\n"
        " as the name is changed.\n"
        "\n"
        "            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE\n"
        "   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION\n"
        "\n"
        "  0. You just DO WHAT THE FUCK YOU WANT TO.\n";

    // Assuming LOGI is a macro or function for logging or displaying information
    LOGI(wtfplLicenseText);
}

void CLI::CompileAndGenerate(bool simulate)
{
    compiler_ = ::std::make_unique<solidity::frontend::CompilerStack>();
    compiler_->setMetadataFormat(solidity::frontend::CompilerStack::MetadataFormat::NoMetadata);
    compiler_->setMetadataHash(solidity::frontend::CompilerStack::MetadataHash::None);

    // set sources
    compiler_->setSources(fileReader_.sourceUnits());

    bool result = compiler_->compile();

    for (auto const &error : compiler_->errors())
    {
        auto sourceLocation = error->sourceLocation();
        ::std::ostringstream ss;
        if (sourceLocation)
        {
            ss << *sourceLocation << "\n";
        }
        ss << error->what();
        switch (error->severity())
        {
        case solidity::langutil::Error::Severity::Warning:
            LOGW("%s", ss.str().c_str());
            break;
        case solidity::langutil::Error::Severity::Error:
            LOGE("%s", ss.str().c_str());
            break;
        case solidity::langutil::Error::Severity::Info:
            LOGI("%s", ss.str().c_str());
            break;
        }
    }

    if (!result)
    {
        LOGE("Error compiling...");
        PSM_BAIL();
    }

    ::std::vector<Report> reports;
    for (auto &x : fileReader_.sourceUnits())
    {
        Report report;
        report.filename = GetFilenameOfPath(x.first);
        LOGI("Parsing %s.sol", report.filename.c_str());
        auto &unit = compiler_->ast(x.first);
        // CPNIDEGenerator generator;
        Generator generator;
        generator.toCPN(unit);
        // generator.dump();

        // get pointer to cpn
        auto network = generator.getNetwork();
        report.dump_path = (options_.input.resultPath / report.filename).string();
        network->setName(report.dump_path);

        // dump csv
        Visualizer::GetInstance().Draw(network, args_.count(g_strVerbose));

        // simulate
        if (simulate)
        {
            Simulator::GetInstance()
                .SetOnStart([&]() {
                // Record the simulation start time in milliseconds since epoch
                report.start_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count();
                LOGI("Simulation Start"); })
                .SetOnEnd([&](){
                // Record the simulation end time in milliseconds since epoch
                report.end_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count();

                // Calculate the duration of the simulation in seconds
                report.duration = (report.end_time - report.start_time) / 1000.0; // Convert from milliseconds to seconds

                LOGI("Simulation End"); })
                .SetOnError([&]() {
                // Log the error
                LOGE("Simulation Error");})
                .SetOnSuccess([&](){
                // Log the success of the simulation
                LOGI("Simulation Successful");
                })
                .Simulate(network);
                reports.push_back(report);
        }
    }

    DumpReports(reports);
    compiler_->reset();
}

po::positional_options_description CLI::GetPositionalOptionsDescription()
{
    // All positional options should be interpreted as input files
    po::positional_options_description filesPositions;
    filesPositions.add(g_strInputFile.c_str(), -1);
    return filesPositions;
}

po::options_description CLI::GetOptionsDescription()
{
    // Declare the supported options.
    po::options_description desc("A automated solidity to CPN and simulation tool...",
                                 po::options_description::m_default_line_length,
                                 po::options_description::m_default_line_length - 23);
    desc.add_options()(g_strHelp.c_str(), "Show help message and exit.")(g_strVersion.c_str(), "Show version and exit.")(g_strLicense.c_str(), "Show licensing information and exit.");

    po::options_description inputOptions("Input Options");
    inputOptions.add_options()(
        g_strBasePath.c_str(),
        po::value<std::string>()->value_name("path"),
        "Use the given path as the root of the source tree instead of the root of the filesystem.")(
        g_strIncludePath.c_str(),
        po::value<std::vector<std::string>>()->value_name("path"),
        "Make an additional source directory available to the default import callback. "
        "Use this option if you want to import contracts whose location is not fixed in relation "
        "to your main source tree, e.g. third-party libraries installed using a package manager. "
        "Can be used multiple times. "
        "Can only be used if base path has a non-empty value.");
    desc.add(inputOptions);

    po::options_description outputOptions("Output Options");
    outputOptions.add_options()(
        (g_strOutputDir + ",o").c_str(),
        po::value<std::string>()->value_name("path"),
        "If given, creates the output file and output products will be put under that folder")(
        g_strEVMVersion.c_str(),
        po::value<std::string>()->value_name("version")->default_value(solidity::langutil::EVMVersion{}.name()),
        "Select desired EVM version. Either homestead, tangerineWhistle, spuriousDragon, "
        "byzantium, constantinople, petersburg, istanbul, berlin, london, paris or shanghai.");
    desc.add(outputOptions);

    desc.add_options()(g_strInputFile.c_str(), po::value<std::vector<std::string>>(), "input file");
    desc.add_options()(g_strSimulate.c_str(), "run simulation");
    desc.add_options()(g_strVerbose.c_str(), "dump unused places and transitions");
    return desc;
}

::std::string CLI::GetFilenameOfPath(::std::string const &_path) const
{
    auto name = boost::filesystem::path(_path).filename().string();
    auto pos = name.find_last_of(".");
    name = name.substr(0, pos);
    return name;
}

void CLI::DumpReports(const ::std::vector<Report>& reports) const {
    ::std::stringstream ss; // Create stringstream object

    auto writer = csv::make_csv_writer(ss); // Create CSV writer

    // Write CSV header
    writer << ::std::vector<::std::string>({
        "Filename", "DumpPath", "StartTime", "EndTime",
        "Duration", "Memory", "InitialHash", "ExpectHash",
        "HashMismatch", "ErrorInfo"
    });

    // Iterate over reports vector and write information of each Report object into the CSV file
    for (const auto& report : reports) {
        writer << ::std::vector<::std::string>({
            report.filename, report.dump_path, ::std::to_string(report.start_time),
            ::std::to_string(report.end_time), ::std::to_string(report.duration), ::std::to_string(report.memory),
            report.initial_hash, report.expect_hash, report.hash_mismatch ? "true" : "false",
            report.error_info
        });
    }

    // Output the content of stringstream to standard output or a file
    LOGI(ss.str().c_str()); // Can also output to a file: ofstream outputFile("reports.csv"); outputFile << ss.str();
}
_END_PSM_NM_