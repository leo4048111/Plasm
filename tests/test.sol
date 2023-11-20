// SPDX-License-Identifier: MIT
pragma solidity >0.4.23;

contract TestContract {
    uint256 public a = 1;
    uint256 public b = 2;

    function foo() public {
        a = b;
    }
}