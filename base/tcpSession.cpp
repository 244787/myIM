#include"TcpSession.h"
#include"muduo/base/Logging.h"
#include<iostream>
#include<json/json.h>
#include<json/value.h>
#include<json/reader.h>
#include<json/writer.h>
#include"chatHandler.h"
TcpSession::TcpSession(muduo::net::EventLoop *loop,
                const muduo::net::InetAddress& listenAddr,
                    const std::string &nameArg):_loop(loop),_server(loop,listenAddr,nameArg)
{
      _server.setConnectionCallback(std::bind(&TcpSession::onConnection,this,std::placeholders::_1));
      _server.setMessageCallback(std::bind(&TcpSession::onMessage,this,std::placeholders::_1,std::placeholders::_2,
      std::placeholders::_3));  

      _server.setThreadNum(4);
}
TcpSession::~TcpSession(){

}
void TcpSession::start(){
    _server.start();
}

void TcpSession::onConnection(const muduo::net::TcpConnectionPtr& conn){
    std::cout<<"in onConnection"<<std::endl;
    if(!conn->connected()){
        conn->shutdown();
    }
}
void TcpSession::onMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buffer,muduo::Timestamp time){

    while (buffer->readableBytes() >= kHeaderLength) // kHeaderLen == 4
    {
      // FIXME: use Buffer::peekInt32()
      const void* data = buffer->peek();
      int32_t be32 = *static_cast<const int32_t*>(data); // SIGBUS//


      const int32_t len = muduo::net::sockets::networkToHost32(be32);
      if (len > 65536 || len < 0)
      {
        LOG_ERROR << "Invalid length " << len;
        conn->shutdown();  // FIXME: disable reading
        break;
      }
      else if (buffer->readableBytes() >= len + kHeaderLength)
      {
        buffer->retrieve(kHeaderLength);
        muduo::string message(buffer->peek(), len);
        //messageCallback_(conn, message, receiveTime);
        Json::Value value;
        Json::Reader reader;
        Json::FastWriter writer;
        if(!reader.parse(message,value)){
            std::cout<<"解析文件失败"<<std::endl;
        }
        std::string str = writer.write(value);
        //std::cout<<"receieve :"<<str; 
        auto handler = chatHandler::GetInstance()->getHandler(value["cmd"].asInt());
        handler(conn,value,time);
        buffer->retrieve(len);
      }
      else
      {
        break;
      }
    }
}