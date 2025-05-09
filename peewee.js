const irc = require('irc'); // Ensure this is only defined once
const fetch = (...args) => import('node-fetch').then(({default: fetch}) => fetch(...args));

const config = {
    channels: ['#schizophrenia'],
    server: 'irc.mellochat.com',
    botName: 'peewee'
};

const greetedUsers = new Set();
const jokedUsers = new Set(); // Set to track users who received a joke
let currentGame = null;

const bot = new irc.Client(config.server, config.botName, {
    channels: config.channels
});

bot.addListener('registered', () => {
    console.log(`Connected to ${config.server} as ${config.botName}`);
});

bot.addListener('message', (from, to, message) => {
    console.log(`${from} => ${to}: ${message}`);

    // Respond to "!hello" command once per user
    if (message === '!hello' && !greetedUsers.has(from)) {
        bot.say(to, `ahoy ${from}, how the shit are ya?`);
        greetedUsers.add(from);
    }

    // Disconnect if user "coffee" types "!gunshot"
    if (from === 'coffee' && message === '!gunshot') {
        bot.disconnect('Goodbye, cruel world...');
    }

    // Start a game of Baccarat
    if (message === '!bac') {
        if (currentGame) {
            bot.say(to, `Sorry ${from}, a game is already in progress.`);
        } else {
            currentGame = new BaccaratGame(from);
            bot.say(to, `${from}, you've started a game of Baccarat! Type '!deal' to deal cards.`);
        }
    }

    if (message === '!deal' && currentGame && currentGame.player === from) {
        const result = currentGame.deal();
        bot.say(to, result);
        currentGame = null;
    }

    // Fahrenheit to Celsius conversion
    if (message.startsWith('!fc ')) {
        const fahrenheit = parseFloat(message.split(' ')[1]);
        if (!isNaN(fahrenheit)) {
            const celsius = ((fahrenheit - 32) * 5) / 9;
            bot.say(to, `${fahrenheit}°F is ${celsius.toFixed(2)}°C`);
        } else {
            bot.say(to, `Please provide a valid number for Fahrenheit, ${from}.`);
        }
    }

    // Celsius to Fahrenheit conversion
    if (message.startsWith('!cf ')) {
        const celsius = parseFloat(message.split(' ')[1]);
        if (!isNaN(celsius)) {
            const fahrenheit = (celsius * 9) / 5 + 32;
            bot.say(to, `${celsius}°C is ${fahrenheit.toFixed(2)}°F`);
        } else {
            bot.say(to, `Please provide a valid number for Celsius, ${from}.`);
        }
    }

    // Fetch a random joke and respond only once per user
    if (message === '!joke') {
        if (!jokedUsers.has(from)) {
            fetch('https://official-joke-api.appspot.com/jokes/random')
                .then(response => response.json())
                .then(data => {
                    const joke = `${data.setup} ${data.punchline}`;
                    bot.say(to, joke);
                    jokedUsers.add(from);
                })
                .catch(error => {
                    console.error(`Error fetching joke: ${error}`);
                    bot.say(to, `Sorry ${from}, I couldn't fetch a joke for you right now.`);
                });
        } else {
            bot.say(to, `You've already received a joke, ${from}. Let someone else have a laugh!`);
        }
    }
});

bot.addListener('ping', (server) => {
    console.log(`Received PING from ${server}`);
    bot.send('PONG', server);
});

bot.addListener('error', (message) => {
    console.error('Error: ', message);
});

class BaccaratGame {
    constructor(player) {
        this.player = player;
        this.deck = this.createDeck();
    }

    createDeck() {
        const suits = ['Hearts', 'Diamonds', 'Clubs', 'Spades'];
        const values = ['2', '3', '4', '5', '6', '7', '8', '9', '10', 'J', 'Q', 'K', 'A'];
        let deck = [];
        for (let suit of suits) {
            for (let value of values) {
                deck.push({ suit, value });
            }
        }
        return this.shuffleDeck(deck);
    }

    shuffleDeck(deck) {
        for (let i = deck.length - 1; i > 0; i--) {
            const j = Math.floor(Math.random() * (i + 1));
            [deck[i], deck[j]] = [deck[j], deck[i]];
        }
        return deck;
    }

    deal() {
        const playerHand = [this.deck.pop(), this.deck.pop()];
        const bankerHand = [this.deck.pop(), this.deck.pop()];
        const playerTotal = this.calculateHandValue(playerHand);
        const bankerTotal = this.calculateHandValue(bankerHand);

        return `Player's hand: ${this.formatHand(playerHand)} (Total: ${playerTotal}) | Banker’s hand: ${this.formatHand(bankerHand)} (Total: ${bankerTotal}) | ${this.determineWinner(playerTotal, bankerTotal)}`;
    }

    calculateHandValue(hand) {
        let value = 0;
        for (let card of hand) {
            if (['J', 'Q', 'K'].includes(card.value)) {
                value += 10;
            } else if (card.value === 'A') {
                value += 1;
            } else {
                value += parseInt(card.value);
            }
        }
        return value % 10;
    }

    formatHand(hand) {
        return hand.map(card => `${card.value} of ${card.suit}`).join(', ');
    }

    determineWinner(playerTotal, bankerTotal) {
        if (playerTotal > bankerTotal) {
            return 'Player wins!';
        } else if (playerTotal < bankerTotal) {
            return 'Banker wins!';
        } else {
            return 'It’s a tie!';
        }
    }
}
