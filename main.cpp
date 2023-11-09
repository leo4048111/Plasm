#include "cli.hpp"

int main(int argc, char** argv)
{
    auto& cli = PSM_NM::CLI::GetInstance();
    cli.Run(argc, argv);
    return 0;
}