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

    function setReward() public payable {
        require (!claimed);

        require(msg.sender == owner);
        // <yes> <report> FRONT_RUNNING
        owner.transfer(reward);
        reward = msg.value;
    }

    function claimReward(uint256 submission) payable public {
        require (!claimed);
        require(submission < 10);
        // <yes> <report> FRONT_RUNNING
        payable(msg.sender).transfer(reward);
        claimed = true;
    }
}
