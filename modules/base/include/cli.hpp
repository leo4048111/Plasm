#pragma once

#include "base.hpp"
#include "singleton.hpp"

#include <memory>

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>

#include <liblangutil/EVMVersion.h>
#include <libsolidity/interface/FileReader.h>
#include <libsolidity/interface/CompilerStack.h>

namespace po = boost::program_options;

_START_SOLIDITY2CPN_NM_

static std::string const g_strHelp = "help";
static std::string const g_strVersion = "version";
static std::string const g_strLicense = "license";
static std::string const g_strBasePath = "base-path";
static std::string const g_strIncludePath = "include-path";
static std::string const g_strOutputDir = "output-dir";
static std::string const g_strEVMVersion = "evm-version";
static std::string const g_strInputFile = "input-file";
static std::string const g_strIgnoreMissingFiles = "ignore-missing";

enum class CLIMode {
	Help = 0,
	License,
	Version,
	NetGen
};

struct CLIOptions {
    struct
	{
        CLIMode mode{CLIMode::NetGen};
		std::set<boost::filesystem::path> paths;
		boost::filesystem::path basePath = "";
		std::vector<boost::filesystem::path> includePaths;
        bool ignoreMissingFiles{false}; // if true, missing input file won't bail the whole cli
	} input;

	struct
	{
		boost::filesystem::path dir = "";
		solidity::langutil::EVMVersion evmVersion;
	} output;

    void Clear() {
        // clear inputs
        this->input.mode = CLIMode::NetGen;
        this->input.paths.clear();
        this->input.basePath = "";
        this->input.includePaths.clear();
        this->input.ignoreMissingFiles = false;

        // clear outputs
        this->output.dir = "";
        this->output.evmVersion = solidity::langutil::EVMVersion{};
    }
};

class CLI {
    SOLIDITY2CPN_SINGLETON(CLI)

public:
    bool Run(int argc, char const* const* argv);

private:
    bool ParseArgs(int argc, char const* const* argv);

    void ProcessArgs();

    void ReadInputFiles();

    void ProcessInput();

    void CompileAndGenerate();

    void PrintHelp() const;

    void PrintLicense() const;

    static boost::program_options::options_description GetOptionsDescription();

    static boost::program_options::positional_options_description GetPositionalOptionsDescription(); 

private:
    CLIOptions options_;
	boost::program_options::variables_map args_;
    solidity::frontend::FileReader fileReader_;
    ::std::unique_ptr<solidity::frontend::CompilerStack> compiler_;
};

_END_SOLIDITY2CPN_NM_