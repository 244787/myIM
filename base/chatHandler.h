#ifndef CHATHANDLER_H_
#define CHATHANDLER_H
#include"Singleton.h"
#include<unordered_map>
#include<json/json.h>
#include<json/value.h>
#include<json/reader.h>
#include<iostream>
#include<muduo/net/TcpConnection.h>
#include<functional>
#include"UserModel.h"
#include"FriendModel.h"
using msgHandlerFnc = std::function<void(const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time )>;


class chatHandler :public Singleton<chatHandler>{
    friend class Singleton<chatHandler>;
public:
    ~chatHandler()=default;
public:
    chatHandler();
    chatHandler(const chatHandler& ) = delete;
    chatHandler operator=(const chatHandler& )= delete;
    void login(const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time );
    void regist(const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time);
    void friend_search(const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time);
    void friend_add_request(const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time);
    void friend_add_respond(const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time);
    msgHandlerFnc getHandler(int CmdId);

    void sendMsgWithHead(const muduo::net::TcpConnectionPtr& _conn, Json::Value& msg );

private:
    std::unordered_map<int, msgHandlerFnc>  msgHandlers;
    std::unordered_map<int,muduo::net::TcpConnection>  userMap;
    UserModel usrMd;
    FriendModel freindMd;


};

#endif

