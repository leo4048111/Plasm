#include <iostream>
#include "cli.hpp"


int main(int argc, char** argv)
{
    auto& cli = SOLIDITY2CPN_NM::CLI::GetInstance();
    cli.Run(argc, argv);
    return 0;
}