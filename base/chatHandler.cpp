#include"chatHandler.h"
#include"msg.h"
#include"connMysql.h"
#include"UserModel.h"
 chatHandler::chatHandler(){
    msgHandlers.insert({static_cast<int>(command::cmd_login),std::bind(&chatHandler::login,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    msgHandlers.insert({static_cast<int>(command::cmd_logout),std::bind(&chatHandler::logout,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    msgHandlers.insert({static_cast<int>(command::cmd_regist),std::bind(&chatHandler::regist,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    msgHandlers.insert({static_cast<int>(command::cmd_friend_search),std::bind(&chatHandler::friend_search,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    msgHandlers.insert({static_cast<int>(command::cmd_add_friend_request),std::bind(&chatHandler::friend_add_request,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    msgHandlers.insert({static_cast<int>(command::cmd_add_friend_response),std::bind(&chatHandler::friend_add_respond,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    msgHandlers.insert({static_cast<int>(command::cmd_friend_list),
        std::bind(&chatHandler::refresh_friend_list,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    msgHandlers.insert({static_cast<int>(command::cmd_friend_chat),
        std::bind(&chatHandler::friend_chat,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    msgHandlers.insert({static_cast<int>(command::cmd_set_icon),
        std::bind(&chatHandler::set_icon,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    msgHandlers.insert({static_cast<int>(command::cmd_offline_message),
        std::bind(&chatHandler::get_offline_message,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});

    
    

 }

void chatHandler::login(const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time ){
    std::cout<<"开始处理登录逻辑\n";
    //登录逻辑
    //读取数据库，判断是否存在此用户
    std::string acc = msg["account"].asString();
    std::string pwd = msg["password"].asString();
    std::cout<<"acc:"<<acc<<"      pwd"<<pwd<<"\n";
    User usr =usrMd.query(acc);
    std::cout<<"accINDB:"<<usr.getAccount()<<"      pwdINDB"<<usr.getPassword()<<"\n";
    if(usr.getPassword()==pwd&&usr.getAccount()==acc){
        //个人信息
        Json::Value res;
        res["cmd"] =  static_cast<int>(command::cmd_login);
        res["res"] = "yes";
        res["nickname"] = usr.getNickName();
        res["icon"] = usr.getIcon();
        res["state"] = "online";        
        sendMsgWithHead(conn,res);

        userMap.insert({acc,conn});
        usrMd.updateState("online",acc);
        
        
    }else{
        Json::Value res;
        res["cmd"] =  static_cast<int>(command::cmd_login);
        res["res"] = "no";
       
        sendMsgWithHead(conn,res);
        
    }
    std::cout<<"登录逻辑处理结束\n";

    //
}
void chatHandler::logout(const muduo::net::TcpConnectionPtr &conn, Json::Value msg, muduo::Timestamp time)
{
    std::cout<<"开始处理下线逻辑";
    std::string account = msg["account_sender"].asString();
    {
        std::unique_lock<std::mutex> locker(mtx);
        auto it = userMap.find(account);
        if(it!=userMap.end())           
            userMap.erase(it);
    }
    //更新在线状态
    usrMd.updateState("offline",account);


    std::cout<<"下线逻辑处理完成";
}
void chatHandler::regist(const muduo::net::TcpConnectionPtr &conn, Json::Value msg, muduo::Timestamp time)
{
    std::cout<<"开始处理注册逻辑"<<std::endl;
    std::string acc = msg["account"].asString();
    std::string pwd = msg["password"].asString();
    std::string nn = msg["nickname"].asString();
    User usr(acc,nn,pwd,"offline");

    if(usrMd.insert(usr)){
        std::cout<<"注册成功！"<<std::endl;
        Json::Value res;
        res["cmd"] =  static_cast<int>(command::cmd_regist);
        res["res"] = "yes";
       
       sendMsgWithHead(conn,res);
    }else{
        std::cout<<"注册失败！"<<std::endl;
        Json::Value res;
        res["cmd"] =  static_cast<int>(command::cmd_regist);
        res["res"] = "no";
        
        sendMsgWithHead(conn,res);
    }
    std::cout<<"注册逻辑处理结束\n";
}
void chatHandler::friend_search(const muduo::net::TcpConnectionPtr &conn, Json::Value msg, muduo::Timestamp time)
{
    std::cout<<"处理查找好友请求中..."<<std::endl;
    std::string fromAcc = msg["account_sender"].asString();
    std::string toAcc = msg["account_reciever"].asString();
    //查找该用户是否存在
    vector<User> usrs;
    if(freindMd.search_friend(toAcc,usrs)){
        //找到了此好友
        Json::Value res;
        res["cmd"] =  static_cast<int>(command::cmd_friend_search);
        res["res"] = "yes";
        for(auto usr:usrs){
            Json::Value js;
            
            js["account"] = usr.getAccount();
            js["nickname"] = usr.getNickName();
            js["status"] = usr.getState();
            res["find"].append(js);
        }
        
        sendMsgWithHead(conn,res);

    }else{
        //未找到此好友
        Json::Value res;
        res["cmd"] =  static_cast<int>(command::cmd_friend_search);
        res["res"] = "no";
        
        sendMsgWithHead(conn,res);
    }
    std::cout<<"好友查找处理结束\n";

}
void chatHandler::friend_add_request(const muduo::net::TcpConnectionPtr &conn, Json::Value msg, muduo::Timestamp time)
{
    std::cout<<"处理添加好友请求。。。"<<std::endl;
    //查看两人是否是好友
    std::string sender = msg["account_sender"].asString();
    std::string receiver = msg["account_reciever"].asString();
    std::string nickname_sender = msg["nickname_sender"].asString();
    char sql[1024];  
    sprintf(sql,"select * from Friend where  userAccount = \"%s\" AND friendAccount = \"%s\" ;",sender.c_str(),receiver.c_str());
    std::cout<<sql<<std::endl;
    connMysql::GetInstance()->Query(sql);
    bool friendExist = (connMysql::GetInstance()->getResult() ==nullptr);
    connMysql::GetInstance()->freeResult();
    if(friendExist){
        //好友已存在在，无法添加。
        Json::Value res;
        res["cmd"] =  static_cast<int>(command::cmd_add_friend_request);
        res["res"] = "no";
        res["reason"] = "exist already";        
        
        sendMsgWithHead(conn,res);

    }else{
        //向用户发出请求
        //查询用户状态
        User rec = usrMd.query(receiver);
        std::string account_reciever = rec.getAccount();
        std::string state = rec.getState();
        Json::Value _res;
            _res["cmd"] =  static_cast<int>(command::cmd_add_friend_request);
            _res["mode"] = "request";
            _res["account_sender"] = sender; 
            _res["nickname_sender"] = nickname_sender; 
            _res["account_reciever"] = account_reciever;
            _res["beizhu"] = msg["beizhu"].asString();
        if(state == "online"){
            //在线，直接向消息          
            auto it =userMap.find(rec.getAccount());

            if(it!=userMap.end())
                sendMsgWithHead(it->second,_res);
            else std::cout<<"好友不在线"<<std::endl;
        }else{
            //离线状态，将消息存在数据库中
            auto it =userMap.find(rec.getAccount());

            offMsgMd.pushMsg(account_reciever,_res.toStyledString());
        }
        //发送请求已发出的信息
        Json::Value res;
        res["cmd"] =  static_cast<int>(command::cmd_add_friend_request);
        res["res"] = "yes";      
        
        sendMsgWithHead(conn,res);
    }
    std::cout<<"添加好友处理结束\n";

}
void chatHandler::friend_add_respond(const muduo::net::TcpConnectionPtr &conn, Json::Value msg, muduo::Timestamp time)
{
    std::cout<<"处理添加好友回复开始"<<std::endl;

    std::string account_reciever = msg["account_reciever"].asString();
    std::string account_sender = msg["account_sender"].asString();
    std::string reply = msg["reply"].asString();
    if(reply == "yes"){
        freindMd.insert(account_reciever,account_sender);
        freindMd.insert(account_sender,account_reciever);
    }else{

    }

    std::cout<<"处理添加好友结束"<<std::endl;

}
void chatHandler::refresh_friend_list(const muduo::net::TcpConnectionPtr &conn, Json::Value msg, muduo::Timestamp time)
{
        //获取好友列表
        std::string acc = msg["account"].asString();
        vector<User> friendVec = freindMd.query(acc);
        Json::Value res;
        for(auto f: friendVec){
            Json::Value fMsg;
            fMsg["account"] = f.getAccount();
            fMsg["nickname"] = f.getNickName();
            fMsg["state"] = f.getState();
            fMsg["icon"] = f.getIcon();
            res["friend"].append(fMsg);
        }
        res["cmd"] = static_cast<int>(command::cmd_friend_list);
        sendMsgWithHead(conn,res);
}
void chatHandler::friend_chat(const muduo::net::TcpConnectionPtr &conn, Json::Value msg, muduo::Timestamp time)
{
    //开始处理好友聊天逻辑
    std::cout<<"开始处理聊天逻辑\n";
    std::string account_reciever = msg["account_reciever"].asString();
    //std::cout<<msg.asString();
    auto it = userMap.find(account_reciever);
    if(it!=userMap.end()){
        sendMsgWithHead(it->second,msg);
    }else{
        offMsgMd.pushMsg(account_reciever,msg.toStyledString());
    }
    std::cout<<"聊天逻辑结束\n";
    //好友聊天逻辑处理j结束
}
void chatHandler::set_icon(const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time){
    //开始处理更换头像逻辑
    std::string url = msg["url"].asString();
    std::string account = msg["account"].asString();
    usrMd.setIcon(url,account);
    //结束处理更换好友逻辑
}
void chatHandler::get_offline_message(const muduo::net::TcpConnectionPtr &conn, Json::Value msg, muduo::Timestamp time)
{
    //离线缓存信息
    std::string acc=msg["account"].asString();
    vector<std::string> vec = offMsgMd.pullMsg(acc);
    for(const auto& v:vec){
        Json::Reader reader;
        Json::Value value;
        reader.parse(v,value);
        sendMsgWithHead(conn,value);
    }
    
}
msgHandlerFnc chatHandler::getHandler(int CmdId)
{
    auto it =msgHandlers.find(CmdId);
    if(it == msgHandlers.end()){
        return [](const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time){
            std::cout<<"error! there is no msgHandlers!\n";
        };
    }
    return msgHandlers[CmdId];
}

void chatHandler::sendMsgWithHead(const muduo::net::TcpConnectionPtr& _conn, Json::Value& msg ){
        std::string JsonRes = msg.toStyledString();
        std::cout<<"jsonREs: "<<JsonRes<<std::endl;
        muduo::net::Buffer buf;
        buf.append(JsonRes.c_str() ,JsonRes.size());
        int32_t len = static_cast<int32_t>(JsonRes.size());
        int32_t be32 = muduo::net::sockets::hostToNetwork32(len);
        buf.prepend(&be32, sizeof be32);
        //std::cout<<buf.toStringPiece().as_string();
        _conn->send(&buf);
}