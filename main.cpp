#include "cli.hpp"

int main(int argc, char** argv)
{
    std::cout << "test output for modeling branch" << std::endl;
    auto& cli = PSM_NM::CLI::GetInstance();
    cli.Run(argc, argv);
    return 0;
}