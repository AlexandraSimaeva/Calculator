#include "http_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sstream>

// Простой HTTP сервер на raw sockets
void HttpEchoServer::runServer() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    //socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return;
    }
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "Setsockopt failed" << std::endl;
        close(server_fd);
        return;
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed on port " << port_ << std::endl;
        close(server_fd);
        return;
    }
    
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        close(server_fd);
        return;
    }
    
    std::cout << "HTTP Echo Server started on port " << port_ << std::endl;
    std::cout << "Listening for connections..." << std::endl;
    
    while (running_) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                                (socklen_t*)&addrlen)) < 0) {
            if (running_) {
                std::cerr << "Accept failed" << std::endl;
            }
            continue;
        }
        
        char buffer[1024] = {0};
        read(new_socket, buffer, 1024);
        
        std::string request(buffer);
        std::string method, path, http_version;
        
        std::istringstream request_stream(request);
        request_stream >> method >> path >> http_version;
        
        std::cout << "Received " << method << " request for " << path << std::endl;
        std::cout << "Full request:\n" << request << std::endl;
        
        std::string response_body = 
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head><title>Echo Server</title></head>\n"
            "<body>\n"
            "    <h1>HTTP Echo Server</h1>\n"
            "    <h2>Request Details:</h2>\n"
            "    <p><strong>Method:</strong> " + method + "</p>\n"
            "    <p><strong>Path:</strong> " + path + "</p>\n"
            "    <p><strong>HTTP Version:</strong> " + http_version + "</p>\n"
            "    <h2>Full Request:</h2>\n"
            "    <pre>" + request + "</pre>\n"
            "</body>\n"
            "</html>";
        
        std::string http_response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + std::to_string(response_body.length()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" + response_body;
        
        // Отправляем ответ
        send(new_socket, http_response.c_str(), http_response.length(), 0);
        
        // Закрываем соединение
        close(new_socket);
        
        std::cout << "Response sent\n" << std::endl;
    }
    
    close(server_fd);
    std::cout << "Server stopped" << std::endl;
}

HttpEchoServer::HttpEchoServer(int port) : port_(port), running_(false) {
    std::cout << "Creating HTTP Echo Server on port " << port_ << std::endl;
}

HttpEchoServer::~HttpEchoServer() {
    stop();
}

void HttpEchoServer::start() {
    if (!running_) {
        running_ = true;
        server_thread_ = std::make_unique<std::thread>(&HttpEchoServer::runServer, this);
        std::cout << "Server starting..." << std::endl;
    }
}

void HttpEchoServer::stop() {
    if (running_) {
        running_ = false;
        if (server_thread_ && server_thread_->joinable()) {
            server_thread_->join();
        }
        std::cout << "Server stopped" << std::endl;
    }
}