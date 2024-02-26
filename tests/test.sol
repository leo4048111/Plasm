// SPDX-License-Identifier: MIT
pragma solidity >0.4.23;

contract TestContract {
    uint256 public a = 1;
    uint256 public b = 2;
    uint256 public c = 4;

    function foo() public {
        c = a + b;

        if(2 > 1) {
            a = 1;
        }
    }
}