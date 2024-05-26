// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

contract VotingContract {
    mapping(address => bool) public voted;
    uint256 public yesCount;
    uint256 public noCount;
    bool public votingClosed = false;

    function vote(bool _vote) public {
        require(voted[msg.sender] == false, "Already voted");
        require(votingClosed == false, "Voting is closed");
        if (_vote) {
            yesCount += 1;
        } else {
            noCount += 1;
        }
        voted[msg.sender] = true;
    }

    function closeVoting() public {
        votingClosed = true;
    }
}
