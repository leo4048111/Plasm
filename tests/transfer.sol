// SPDX-License-Identifier: MIT
pragma solidity >0.4.23;

// ether transfer with address.transfer
contract ExampleTransfer {
    address payable owner;

    function foo() public {
        owner.transfer(1 ether);
    }
}

// ether transfer with a globally defined transfer function
contract ExampleTransfer2 {
    mapping(address => uint256) balances;
    address payable owner;

    function transfer(address payable addr, uint256 amount) public
    {
        balances[addr] += amount;
    }

    function foo() public {
        transfer(owner, 1);
    }
}