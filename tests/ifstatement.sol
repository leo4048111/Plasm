// SPDX-License-Identifier: MIT
pragma solidity >0.4.23;

contract Baseline {
    function greater(uint256 x, uint256 y) public pure returns (bool result) {
        if(x > y) {
            result = true;
        } else {
            result = false;
        }
    }
}
