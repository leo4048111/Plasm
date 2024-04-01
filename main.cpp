#include "cli.hpp"
#include "logger.hpp"

int main(int argc, char** argv)
{
#ifdef DEBUG
    PSM_NM::Logger::GetInstance().SetLevel(PSM_NM::LogLevel::LInfo);
#endif

    auto& cli = PSM_NM::CLI::GetInstance();
    cli.Run(argc, argv);
    return 0;
}