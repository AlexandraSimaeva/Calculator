#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <thread>
#include <memory>
#include <string>

class HttpEchoServer {
private:
    int port_;
    bool running_;
    std::unique_ptr<std::thread> server_thread_;
    
    void runServer();
    
public:
    HttpEchoServer(int port = 8080);
    ~HttpEchoServer();
    
    void start();
    void stop();
    bool isRunning() const;
};

#endif