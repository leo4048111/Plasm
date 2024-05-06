// SPDX-License-Identifier: MIT
pragma solidity >0.4.23;

contract Baseline {
    uint256 a; // global value declaration
    uint256 b = 1 + 1;
    // function definition
    function foo() public pure
    {
        uint256 c = 1; // local value declaration
    }
}

