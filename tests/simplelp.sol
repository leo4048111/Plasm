// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

contract SimpleLP {
    uint256 balance1 = 0;
    uint256 balance2 = 0;

    function deposit(uint256 amount1, uint256 amount2) public {
        require(amount1 * balance2 == amount2 * balance1);
        balance1 += amount1;
        balance2 += amount2;
    }

    function swap1for2(uint256 amount1) public {
        require(balance1 >= amount1);

        uint256 amount2 = amount1 * balance2 / balance1;
        require(balance2 >= amount2);

        balance1 -= amount1;
        balance2 += amount2;
    }

    function swap2for1(uint256 amount2) public {
        require(balance2 >= amount2);

        uint256 amount1 = amount2 * balance1 / balance2;
        require(balance1 >= amount1);

        balance2 -= amount2;
        balance1 += amount1;
    }
}
