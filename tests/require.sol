// SPDX-License-Identifier: MIT
pragma solidity >0.4.23;

contract Baseline {
    uint256 value = 1;
    function foo() public view
    {
        require(value > 1, "Value must be greater than 1");
    }
}

