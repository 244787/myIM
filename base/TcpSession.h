
#include"muduo/net/TcpServer.h"
#include<mutex>
#include<memory>
#include"muduo/net/EventLoop.h"


class TcpSession{
public:
    TcpSession(muduo::net::EventLoop *loop,
                const muduo::net::InetAddress& lsitenAddr,
                    const std::string &nameArg);
    ~TcpSession();
    TcpSession(const TcpSession& ) = delete;
    void start();
private:
    void onConnection(const muduo::net::TcpConnectionPtr&);
    void onMessage(const muduo::net::TcpConnectionPtr& ,muduo::net::Buffer* ,muduo::Timestamp );
private:
    muduo::net::EventLoop *_loop;
    muduo::net::TcpServer _server;
    int kHeaderLength = 4;

};
