#include "http_server.h"
#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>
#include <string>
#include <sstream>
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

HttpEchoServer* server = nullptr;

void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    if (server) {
        server->stop();
    }
}
int main()
{
    std::cout << "=== C++ HTTP Echo Server ===\n" << std::endl;
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    HttpEchoServer echoServer(9090);
    server = &echoServer;
    
    echoServer.start();
    
    std::cout << "\nServer is running on http://localhost:9090" << std::endl;
    std::cout << "Press Ctrl+C to stop the server\n" << std::endl;
    
    // Бесконечный цикл или ожидание
    while (echoServer.isRunning()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    std::cout << "Server shutdown complete." << std::endl;
    return 0;
}