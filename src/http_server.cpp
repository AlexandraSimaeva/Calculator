#include "http_server.h"
#include "Calc.h"   
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <regex>
#include <iostream>
#include <string>

std::string parseJSON(const std::string& json, const std::string& key) 
{
    std::regex pattern("\"" + key + "\"\\s*:\\s*\"?([^,\"}]+)\"?");
    std::smatch match;
    
    if (std::regex_search(json, match, pattern) && match.size() > 1) 
    {
        return match[1].str();  // Добавили .str()
    }
    return "";
}

std::string createResponse(const std::string& value) 
{
    return "{\"res\": \"" + value + "\"}";
}

std::string createResponse(double value) 
{
    std::stringstream ss;
    ss << "{\"res\": " << value << "}";
    return ss.str();
}

void HttpEchoServer::runServer() 
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        std::cerr << "Socket creation failed" << std::endl;
        return;
    }
    
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) 
    {
        std::cerr << "Setsockopt failed" << std::endl;
        close(server_fd);
        return;
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        std::cerr << "Bind failed on port " << port_ << std::endl;
        close(server_fd);
        return;
    }
    
    if (listen(server_fd, 3) < 0) 
    {
        std::cerr << "Listen failed" << std::endl;
        close(server_fd);
        return;
    }
    
    std::cout << "Calculator HTTP Server started on port " << port_ << std::endl;
    std::cout << "Supported requests:" << std::endl;
    std::cout << "  {\"cmd\": \"echo\"} - echo test" << std::endl;
    std::cout << "  {\"exp\": \"2+2\"}  - calculate expression" << std::endl;
    std::cout << "Listening for connections..." << std::endl;
    
    Calculator calculator;  
    
    while (running_) 
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                                (socklen_t*)&addrlen)) < 0) 
        {
            if (running_) 
            {
                std::cerr << "Accept failed" << std::endl;
            }
            continue;
        }
        
        char buffer[4096] = {0};
        ssize_t bytes_read = read(new_socket, buffer, 4095);
        if (bytes_read <= 0) {
            close(new_socket);
            continue;
        }
        
        std::string request(buffer, bytes_read);
        
        size_t json_start = request.find('{');
        size_t json_end = request.find_last_of('}');

        std::string body;
        if (json_start != std::string::npos && json_end != std::string::npos && json_end > json_start) 
        {
            body = request.substr(json_start, json_end - json_start + 1);
            std::cout << "Found JSON body: " << body << std::endl;
        } 
        else 
        {
            body = request;   
            std::cout << "No JSON found, using full request: " << body << std::endl;
        }
        
        std::string response_body;
        
        std::string cmd = parseJSON(body, "cmd");
        std::string exp = parseJSON(body, "exp");
        if (!cmd.empty() && cmd == "echo") 
        {
            response_body = createResponse("echo");
        }
        else if (!exp.empty()) 
        {
            try 
            {
                double result = calculator.evaluate(exp);
                response_body = createResponse(result);
            }
            catch (const std::exception& e) 
            {
                response_body = createResponse(e.what());
            }
        }
        else 
        {
            response_body = createResponse("Invalid request format");
        }
     
        std::string http_response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: " + std::to_string(response_body.length()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" + response_body;
        
        send(new_socket, http_response.c_str(), http_response.length(), 0);
        close(new_socket);
        
        std::cout << "Response: " << response_body << std::endl;
    }
    
    close(server_fd);
    std::cout << "Server stopped" << std::endl;
}

HttpEchoServer::HttpEchoServer(int port) : port_(port), running_(false) 
{
    std::cout << "Creating Calculator HTTP Server on port " << port_ << std::endl;
}

HttpEchoServer::~HttpEchoServer() 
{
    stop();
}

void HttpEchoServer::start() 
{
    if (!running_) 
    {
        running_ = true;
        server_thread_ = std::make_unique<std::thread>(&HttpEchoServer::runServer, this);
        std::cout << "Server starting..." << std::endl;
    }
}

void HttpEchoServer::stop() 
{
    if (running_) 
    {
        running_ = false;
        if (server_thread_ && server_thread_->joinable()) 
        {
            server_thread_->join();
        }
        std::cout << "Server stopped" << std::endl;
    }
}

bool HttpEchoServer::isRunning() const {
    return running_;
}