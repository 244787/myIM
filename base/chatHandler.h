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
#include"offlineMsgModel.h"
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
    void logout(const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time );
    void regist(const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time);
    void friend_search(const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time);
    void friend_add_request(const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time);
    void friend_add_respond(const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time);
    void refresh_friend_list(const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time);
    void friend_chat(const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time);
    void set_icon(const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time);
    void get_offline_message(const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time);
    msgHandlerFnc getHandler(int CmdId);

    void sendMsgWithHead(const muduo::net::TcpConnectionPtr& _conn, Json::Value& msg );

private:
    std::unordered_map<int, msgHandlerFnc>  msgHandlers;
    std::unordered_map<std::string,muduo::net::TcpConnectionPtr>  userMap;
    UserModel usrMd;
    FriendModel freindMd;
    offlineMsgModel offMsgMd;
    std::mutex mtx;


};

#endif

