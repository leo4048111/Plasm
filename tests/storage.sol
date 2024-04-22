// SPDX-License-Identifier: GPL-3.0

pragma solidity >=0.7.0 <0.9.0;

/**
 * @title Storage
 * @dev Store & retrieve value in a variable
 * @custom:dev-run-script ./scripts/deploy_with_ethers.ts
 */
contract Storage {

    uint256 number;

    /**
     * @dev Store value in variable
     * @param num value to store
     */
    function store(uint256 num) public {
        uint256 test_v;
        number = num;
        test_v = num + 1;
    }

    /**
     * @dev Return value 
     * @return value of 'number'
     */
    function retrieve(uint256 p1, uint256 p2) public view returns (uint256){
        p1 + p2;
        return number;
    }
}