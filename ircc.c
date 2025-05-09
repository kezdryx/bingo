#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <netdb.h>

#define BUFFER_SIZE 512
#define SERVER_WIN 0
#define CHANNEL_WIN 1

int sockfd;
WINDOW *server_win, *channel_win, *input_win;
int active_win = SERVER_WIN;
int running = 1;

void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    while (running) {
        memset(buffer, 0, sizeof(buffer));
        if (recv(sockfd, buffer, sizeof(buffer) - 1, 0) <= 0) {
            break;
        }
        if (strncmp(buffer, "PING", 4) == 0) {
            char response[BUFFER_SIZE];
            snprintf(response, sizeof(response), "PONG %s\r\n", buffer + 5);
            send(sockfd, response, strlen(response), 0);
            wprintw(server_win, "PING received. PONG sent.\n");
        } else {
            wprintw(active_win == SERVER_WIN ? server_win : channel_win, "%s", buffer);
        }
        wrefresh(active_win == SERVER_WIN ? server_win : channel_win);
    }
    return NULL;
}

void switch_windows() {
    active_win = (active_win == SERVER_WIN) ? CHANNEL_WIN : SERVER_WIN;
    wprintw(active_win == SERVER_WIN ? server_win : channel_win, "Switched windows.\n");
    wrefresh(server_win);
    wrefresh(channel_win);
}

int main() {
    struct sockaddr_in server_addr;
    struct hostent *server;
    char input[BUFFER_SIZE], buffer[BUFFER_SIZE];

    // Initialize ncurses
    initscr();
    cbreak();
    noecho();

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    server_win = newwin(rows / 2, cols, 0, 0);
    channel_win = newwin(rows / 2, cols, rows / 2, 0);
    input_win = newwin(3, cols, rows - 3, 0);

    scrollok(server_win, TRUE);
    scrollok(channel_win, TRUE);

    // Connect to the server
    const char *hostname = "irc.mellochat.com";
    int port = 6667;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname(hostname);

    if (!server) {
        perror("Error: Host not found");
        endwin();
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        endwin();
        return 1;
    }

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);

    // Register and join a channel
    send(sockfd, "NICK SimpleClient\r\n", 19, 0);
    send(sockfd, "USER SimpleClient 0 * :A Simple IRC Client\r\n", 43, 0);
    send(sockfd, "JOIN #techtalk\r\n", 15, 0);

    while (running) {
        wprintw(input_win, "> ");
        wrefresh(input_win);
        memset(input, 0, sizeof(input));
        wgetnstr(input_win, input, sizeof(input) - 1);

        if (strcmp(input, "/quit") == 0) {
            running = 0;
            break;
        } else if (strcmp(input, "/wn") == 0) {
            switch_windows();
        } else {
            snprintf(buffer, sizeof(buffer), "PRIVMSG #techtalk :%s\r\n", input);
            send(sockfd, buffer, strlen(buffer), 0);
            wprintw(channel_win, "You: %s\n", input);
            wrefresh(channel_win);
        }
        werase(input_win);
    }

    pthread_cancel(recv_thread);
    close(sockfd);
    endwin();
    return 0;
}
