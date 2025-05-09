const fs = require('fs');
const irc = require('irc');

const client = new irc.Client('irc.example.com', 'TicTacToeBot', {
    channels: ['#channel'],
});

let players = [];
let board = Array(9).fill(null);
let currentPlayerIndex = 0;
let gameActive = false;
let playerStats = {};

function printBoard(channel) {
    const displayBoard = board.map((cell, index) => cell || (index + 1).toString()).join('|');
    const formattedBoard = `${displayBoard.slice(0, 5)}\n${displayBoard.slice(6, 11)}\n${displayBoard.slice(12)}`;
    client.say(channel, `\n${formattedBoard}`);
}

function checkWin() {
    const winPatterns = [
        [0, 1, 2], [3, 4, 5], [6, 7, 8],
        [0, 3, 6], [1, 4, 7], [2, 5, 8],
        [0, 4, 8], [2, 4, 6]
    ];
    return winPatterns.some(pattern => {
        const [a, b, c] = pattern;
        return board[a] && board[a] === board[b] && board[a] === board[c];
    });
}

function updateStats(winner, loser, draw = false) {
    if (draw) {
        playerStats[players[0]].points += 0.5;
        playerStats[players[1]].points += 0.5;
        playerStats[players[0]].games += 1;
        playerStats[players[1]].games += 1;
    } else {
        playerStats[winner].points += 1;
        playerStats[loser].points += 0;
        playerStats[winner].games += 1;
        playerStats[loser].games += 1;
    }
    saveStats();
}

function saveStats() {
    const statsData = Object.entries(playerStats).map(([nick, { points, games }]) =>
        `${nick}: ${points} points, ${games} games`
    ).join('\n');
    fs.writeFileSync('playerStats.txt', statsData);
}

function sendGreeting(channel) {
    client.say(channel, "Who wants to play Tic Tac Toe? Be the first or second person to type !play.");
}

function resetGame(channel) {
    players = [];
    board = Array(9).fill(null);
    currentPlayerIndex = 0;
    gameActive = false;
    sendGreeting(channel);
}

client.addListener('message', (from, to, message) => {
    if (message === 'ping') {
        client.say(to, 'pong');
    }

    if (message === '!play') {
        if (players.length < 2 && !players.includes(from)) {
            players.push(from);
            if (!playerStats[from]) {
                playerStats[from] = { points: 0, games: 0 };
            }
            client.say(to, `${from} has joined the game as Player ${players.length}`);
            if (players.length === 2) {
                gameActive = true;
                client.say(to, `${players[0]} (X) vs ${players[1]} (O) - Let's start!`);
                printBoard(to);
            }
        }
    }

    if (gameActive && players.includes(from)) {
        const playerIndex = players.indexOf(from);
        if (playerIndex === currentPlayerIndex) {
            const move = parseInt(message, 10);
            if (move >= 1 && move <= 9 && !board[move - 1]) {
                board[move - 1] = currentPlayerIndex === 0 ? 'X' : 'O';
                printBoard(to);

                if (checkWin()) {
                    client.say(to, `${players[currentPlayerIndex]} wins!`);
                    updateStats(players[currentPlayerIndex], players[1 - currentPlayerIndex]);
                    gameActive = false;
                    setTimeout(() => resetGame(to), 5000); // 5-second delay before resetting the game
                } else if (board.every(cell => cell)) {
                    client.say(to, `It's a draw!`);
                    updateStats(players[0], players[1], true);
                    gameActive = false;
                    setTimeout(() => resetGame(to), 5000); // 5-second delay before resetting the game
                } else {
                    currentPlayerIndex = 1 - currentPlayerIndex;
                }
            }
        }
    }

    if (from === 'bot owner' && message === '!exit') {
        client.say(to, 'Goodbye!');
        client.disconnect();
    }
});

client.addListener('registered', (message) => {
    setTimeout(() => {
        sendGreeting(client.opt.channels[0]);
    }, 8000); // 8000 milliseconds = 8 seconds
});
