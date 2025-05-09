#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

std::string nick = "yournick";
std::string nick_password = ".";
std::string channel = "#channel";

void identify_password(int sockfd) {
    std::string nick_command = "PRIVMSG NICKSERV :identify " + nick_password + "\r\n";
    send(sockfd, nick_command.c_str(), nick_command.size(), 0);
}

void join_channel(int sockfd, const std::string& channel) {
    std::string join_command = "JOIN " + channel + "\r\n";
    send(sockfd, join_command.c_str(), join_command.size(), 0);
}

void set_nickname(int sockfd, const std::string& nickname) {
    std::string user_command = "USER " + nickname + " " + nickname + " " + nickname + ":just testing\r\n";
    send(sockfd, user_command.c_str(), user_command.size(), 0);
    
    std::string nick_command = "NICK " + nickname + "\r\n";
    send(sockfd, nick_command.c_str(), nick_command.size(), 0);
}

void read_from_server(int sockfd, const std::string& out_file) {
    std::ofstream out(out_file);
    char buffer[512];
    int count = 0;
    bool first = true;
    while (true) {
        ssize_t bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) break;
        buffer[bytes_received] = '\0';
        out << buffer;
        out.flush();
        count++;
        if (count == 2) {
            set_nickname(sockfd, nick);
        }
        std::string message(buffer);
        if (message.find("PING") != std::string::npos) {
            std::string response = "PONG " + message.substr(5) + "\r\n";
            send(sockfd, response.c_str(), response.size(), 0);
            if (first) {
                identify_password(sockfd);
                join_channel(sockfd, channel);
                first = false;
            }
        }
    }
}

void write_to_server(int sockfd, const std::string& in_file) {
    std::ifstream in(in_file);
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line[0] == '/') {
            // Handle commands
            std::string command = line.substr(1, line.find(' ') - 1);
            std::string content = line.substr(line.find(' ') + 1);
            
            if (command == "me") {
                line = "PRIVMSG " + channel + " :\001ACTION " + content + "\001\r\n";
            } else if (command == "nick") {
                line = "NICK " + content + "\r\n";
                nick = content;
            } else if (command == "join") {
                line = "JOIN " + content + "\r\n";
                channel = content;
            } else if (command == "part") {
                line = "PART " + channel + " :" + content + "\r\n";
            } else if (command == "quit") {
                line = "QUIT :" + content + "\r\n";
                send(sockfd, line.c_str(), line.size(), 0);
                break;
            } else {
                std::cout << "Unknown command: " << command << std::endl;
                continue;
            }
        } else {
            // Regular message
            line = "PRIVMSG " + channel + " :" + line + "\r\n";
        }
        
        std::cout << "Sending: " << line << std::endl;
        send(sockfd, line.c_str(), line.size(), 0);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    const std::string server = "irc.example.com";
    const std::string port = "6667";
    const std::string nickname = "yournick";
    const std::string in_file = "in.txt";
    const std::string out_file = "out.txt";
    const std::string channel = "#channel";
    struct addrinfo hints = {}, *res;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(server.c_str(), port.c_str(), &hints, &res);
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    connect(sockfd, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
    std::thread reader(read_from_server, sockfd, out_file);
    std::thread writer(write_to_server, sockfd, in_file);
    
    writer.join();  
    
    // Close the socket and end the reader thread
    close(sockfd);
    reader.detach();  
    
    return 0;
}