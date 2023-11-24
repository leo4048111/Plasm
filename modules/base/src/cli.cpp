#include "cli.hpp"

#include <string>
#include <optional>
#include <set>
#include <map>

#include "logger.hpp"
#include "noopgenerator.hpp"
#include "cpnidegenerator.hpp"

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
            if (!options_.input.ignoreMissingFiles) {
                LOGE("\"%s\" is not found.", infile.string().c_str());
                PSM_BAIL();
            }
            else
                LOGW("\"%s\" is not found. Skipping.", infile.string().c_str());

            continue;
        }

        if (!boost::filesystem::is_regular_file(infile))
        {
            if (!options_.input.ignoreMissingFiles) {
                LOGE("%s%s%s", '"', infile.string().c_str(), "\" is not a valid regular file.");
                PSM_BAIL();
            }
            else
                LOGW("%s is not a valid regular file. Skipping.", infile.string().c_str());

            continue;
        }

        ::std::string fileContent = solidity::util::readFileAsString(infile);
        fileReader_.addOrUpdateFile(infile, ::std::move(fileContent));
        fileReader_.allowDirectory(boost::filesystem::canonical(infile).remove_filename());
    }

    if (fileReader_.sourceUnits().empty())
    {
        LOGE("All specified input files either do not exist or are not regular files.");
        PSM_BAIL();
    }
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
        " Copyright (C) [2023] [Z3r0M3r0ry]\n"
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

void CLI::CompileAndGenerate()
{
    compiler_ = ::std::make_unique<solidity::frontend::CompilerStack>();
    compiler_->setMetadataFormat(solidity::frontend::CompilerStack::MetadataFormat::NoMetadata);
    compiler_->setMetadataHash(solidity::frontend::CompilerStack::MetadataHash::None);

    // set sources
    compiler_->setSources(fileReader_.sourceUnits());

    bool result = compiler_->compile();

    for (auto const &error : compiler_->errors())
    {
        LOGW("%s", error->what());
    }

    if(!result) {
        LOGE("Error compiling...");
        PSM_BAIL();
    }

    for(auto& x : fileReader_.sourceUnits()) {
        auto& unit = compiler_->ast(x.first);
        CPNIDEGenerator generator;
        generator.toCPN(unit);
        generator.dump();
    }

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
    po::options_description desc("FUck any smart contract you like...",
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
    return desc;
}

_END_PSM_NM_