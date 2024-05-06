// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

contract SimplePayment {
    mapping(address => uint256) public balances;

    // Deposit money into the contract
    function deposit() public payable {
        balances[msg.sender] += msg.value;
    }

    // Withdraw money from the contract
    function withdraw(uint256 amount) public {
        require(balances[msg.sender] >= amount, "Insufficient balance");
        balances[msg.sender] -= amount;
        // Instead of sending Ether, adjusting balance
    }
}
