const irc = require('irc');

const bot = new irc.Client('irc.mellochat.com', 'MrTexas', {
    channels: ['#texasholdem'],
});

const deck = [];
const players = {};
const communityCards = [];
const bank = {};
const currentBets = {};
const maxPlayers = 4;
const startingCredits = 10000;
let activePlayers = [];
let smallBlind = 50;
let bigBlind = 100;
let turnIndex = 0;
let roundStage = 'pre-flop';

function initializeDeck() {
    const suits = ['♥', '♦', '♠', '♣'];
    const ranks = ['2', '3', '4', '5', '6', '7', '8', '9', '10', 'J', 'Q', 'K', 'A'];
    deck.length = 0;
    suits.forEach(suit => {
        ranks.forEach(rank => {
            deck.push(`${rank}${suit}`);
        });
    });
}

function shuffleDeck() {
    for (let i = deck.length - 1; i > 0; i--) {
        const j = Math.floor(Math.random() * (i + 1));
        [deck[i], deck[j]] = [deck[j], deck[i]];
    }
}

function dealCards() {
    Object.keys(players).forEach(nick => {
        players[nick] = [deck.pop(), deck.pop()];
        bot.say(nick, `Your private cards: ${players[nick].join(', ')}`);
    });
}

function revealFlop() {
    communityCards.length = 0;
    for (let i = 0; i < 3; i++) {
        communityCards.push(deck.pop());
    }
    bot.say('#texasholdem', `Flop: ${communityCards.join(', ')}`);
    roundStage = 'flop';
}

function revealTurn() {
    communityCards.push(deck.pop());
    bot.say('#texasholdem', `Turn: ${communityCards[3]}`);
    roundStage = 'turn';
}

function revealRiver() {
    communityCards.push(deck.pop());
    bot.say('#texasholdem', `River: ${communityCards[4]}`);
    roundStage = 'river';
}

function enforceBlinds() {
    const playerKeys = Object.keys(players);
    if (playerKeys.length >= 2) {
        bank[playerKeys[0]] -= smallBlind;
        currentBets[playerKeys[0]] = smallBlind;
        bank[playerKeys[1]] -= bigBlind;
        currentBets[playerKeys[1]] = bigBlind;
        bot.say('#texasholdem', `${playerKeys[0]} posts Small Blind (${smallBlind}). ${playerKeys[1]} posts Big Blind (${bigBlind}).`);
    }
}

function evaluateHands() {
    let bestHandStrength = 0;
    let winner = null;

    Object.keys(players).forEach(nick => {
        const fullHand = [...players[nick], ...communityCards];
        const strength = getHandStrength(fullHand);
        if (strength > bestHandStrength) {
            bestHandStrength = strength;
            winner = nick;
        }
    });

    return winner;
}

function finalizeRound() {
    const winner = evaluateHands();
    bot.say('#texasholdem', `${winner} wins this round!`);
    bank[winner] += Object.values(currentBets).reduce((sum, bet) => sum + bet, 0);
    currentBets = {};
    roundStage = 'pre-flop';
}

function nextTurn() {
    if (activePlayers.length > 0) {
        bot.say('#texasholdem', `It's ${activePlayers[turnIndex]}'s turn to act.`);
    }
}

bot.addListener('message', (from, to, message) => {
    const args = message.split(' ');
    const command = args[0].toLowerCase();

    if (command === '!join') {
        if (Object.keys(players).length < maxPlayers) {
            players[from] = [];
            bank[from] = startingCredits;
            activePlayers.push(from);
            bot.say(from, `You've joined the game with ${startingCredits} credits!`);
        } else {
            bot.say(from, 'Game is full!');
        }
    } else if (command === '!deal') {
        if (Object.keys(players).length > 1) {
            initializeDeck();
            shuffleDeck();
            dealCards();
            enforceBlinds();
            bot.say('#texasholdem', `Starting pre-flop betting round.`);
        } else {
            bot.say('#texasholdem', 'Not enough players to start.');
        }
    } else if (command === '!bet') {
        const amount = parseInt(args[1], 10);
        if (!isNaN(amount) && bank[from] >= amount) {
            bank[from] -= amount;
            currentBets[from] = amount;
            bot.say('#texasholdem', `${from} bet ${amount} credits.`);
        } else {
            bot.say(from, 'Invalid bet or insufficient funds.');
        }
    } else if (command === '!fold') {
        delete players[from];
        bot.say('#texasholdem', `${from} has folded.`);
        if (Object.keys(players).length === 1) {
            finalizeRound();
        }
    } else if (command === '!next') {
        turnIndex = (turnIndex + 1) % activePlayers.length;
        nextTurn();
    } else if (command === '!showdown') {
        if (roundStage === 'river') {
            finalizeRound();
        } else {
            bot.say('#texasholdem', 'The game is not at the showdown stage yet.');
        }
    }
});

bot.addListener('error', message => {
    console.error('Error:', message);
});