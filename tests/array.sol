// SPDX-License-Identifier: MIT
pragma solidity >0.4.23;

contract Baseline {
    uint[] public balances = [1, 2, 3];
    address[4] public addresses;
    uint a = 0;
    bool condition = false;
    function foo() public {
        if(condition)
            a = balances[0];
        else
            a = balances[1];
    }

    function foo2() public {
        condition = true;
    }
}
