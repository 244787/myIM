#include"TcpSession.h"
#include<iostream>
TcpSession::TcpSession(muduo::net::EventLoop *loop,
                const muduo::net::InetAddress& listenAddr,
                    const std::string &nameArg):_loop(loop),_server(loop,listenAddr,nameArg)
{
      _server.setConnectionCallback(std::bind(&TcpSession::onConnection,this,std::placeholders::_1));
      _server.setMessageCallback(std::bind(&TcpSession::onMessage,this,std::placeholders::_1,std::placeholders::_2,
      std::placeholders::_3));  
}
TcpSession::~TcpSession(){

}
void TcpSession::start(){

}

void TcpSession::onConnection(const muduo::net::TcpConnectionPtr& conn){
    std::cout<<"in onConnection"<<std::endl;
}
void TcpSession::onMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buffer,muduo::Timestamp time){
    std::cout<<" inMessage"<<std::endl;
}