// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

contract ConditionalReward {
    mapping(address => uint256) public points;
    bool public rewardAvailable = false;

    function earnPoints() public {
        points[msg.sender] += 10;
    }

    function enableRewards() public {
        rewardAvailable = true;
    }

    function claimReward() public {
        require(rewardAvailable, "Rewards are not available yet");
        require(points[msg.sender] >= 100, "Not enough points");
        points[msg.sender] -= 100; // Deduct points on reward claim
        // No actual transfer of Ether to simplify and focus on state change
    }
}
