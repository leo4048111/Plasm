// SPDX-License-Identifier: MIT
pragma solidity >0.4.23;

contract Baseline {
    // function definition
    function add(uint256 x, uint256 y) public pure returns(uint256 ret)
    {
        ret = x + y;
        return ret;
    }

    function sub(uint256 x, uint256 y) public pure returns(uint256 ret)
    {
        ret = x - y;
        return ret;
    }
}

