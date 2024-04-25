// SPDX-License-Identifier: MIT
pragma solidity >0.4.23;

contract Baseline {
    mapping (address => uint256) public balances;
    bool condition = false;
    function foo() public {
        uint256 a = balances[msg.sender];
        if(condition)
            balances[msg.sender] = 999;
    }

    function foo2() public {
        condition = true;
    }
}
