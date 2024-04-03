// SPDX-License-Identifier: MIT
pragma solidity >0.4.23;

contract Baseline {
    function increment(uint256 x) public pure returns(uint256 result){
        while(x < 0) {
            x = x + 1;
        }

        result = x;
    }
}
