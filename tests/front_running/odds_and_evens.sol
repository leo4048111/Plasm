pragma solidity >0.4.2;

contract OddsAndEvens {
    mapping(address => uint) public balances;
    mapping(address => uint) public numbers;
    address private player1Addr;
    address private player2Addr;

    uint8 public tot;

    // Function for players to play the game
    function play(uint number) public payable {
        if (msg.value != 1 ether) return;
        if (tot == 0) {
            player1Addr = msg.sender;
            numbers[player1Addr] = number;
            tot += 1;
        } else if (tot == 1) {
            player2Addr = msg.sender;
            numbers[player2Addr] = number;
            tot += 1;
            if (tot == 2) andTheWinnerIs();
        }
    }

    // Private function to decide and reward the winner
    function andTheWinnerIs() private {
        uint n = numbers[player1Addr] + numbers[player2Addr];
        if (n % 2 == 0) {
            balances[player1Addr] += 1800;
        } else {
            balances[player2Addr] += 1800;
        }

        tot = 0;
    }
}
