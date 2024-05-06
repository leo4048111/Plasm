// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

contract SimpleContract {
    function firstFunction(uint256 input) public view returns (uint256) {
        return secondFunction(input + 1);
    }

    function secondFunction(uint256 input) public pure returns (uint256) {
        return input;
    }
}
