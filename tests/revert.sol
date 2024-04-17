// SPDX-License-Identifier: MIT
pragma solidity >0.4.23;

contract Baseline {
    function add(uint256 x, uint256 y) public pure returns (uint256 ret) {
        x + y;
    }

    function sub(uint256 x, uint256 y) public pure returns (uint256 ret) {
        uint256 z = x - y;
    }
}
