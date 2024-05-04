/*
 * @source: https://github.com/ConsenSys/evm-analyzer-benchmark-suite
 * @author: Suhabe Bugrara
 * @vulnerable_at_lines: 23,31
 */

pragma solidity >0.4.16;

contract EthTxOrderDependenceMinimal {
    address payable public owner;
    bool public claimed;
    uint public reward;
    mapping(address => uint) public balances;

    function setReward() public payable {
        require (claimed != false);

        require(msg.sender == owner);
        // <yes> <report> FRONT_RUNNING
        reward = 9999;
    }

    function claimReward(uint256 submission) payable public {
        require (claimed != false);
        require(submission < 10);
        // <yes> <report> FRONT_RUNNING
        balances[msg.sender] += reward;
        claimed = true;
    }
}
