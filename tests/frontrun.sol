// SPDX-License-Identifier: MIT
pragma solidity >0.4.23;

contract Baseline {
    uint public x = 0;
    uint public y = 0;
    function foo1() public
    {
        if(x == 0)
        {
            y = 100;
        } else {
            y = 50;
        }
    }

    function foo2() public
    {
        x = 1;
    }
}

