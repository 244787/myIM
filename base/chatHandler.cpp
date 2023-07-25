#include"chatHandler.h"
#include"msg.h"
#include"connMysql.h"
#include"UserModel.h"
 chatHandler::chatHandler(){
    msgHandlers.insert({static_cast<int>(command::cmd_login),std::bind(&chatHandler::login,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    msgHandlers.insert({static_cast<int>(command::cmd_logout),std::bind(&chatHandler::login,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    msgHandlers.insert({static_cast<int>(command::cmd_regist),std::bind(&chatHandler::regist,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    msgHandlers.insert({static_cast<int>(command::cmd_friend_search),std::bind(&chatHandler::friend_search,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    msgHandlers.insert({static_cast<int>(command::cmd_add_friend_request),std::bind(&chatHandler::friend_add_request,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    msgHandlers.insert({static_cast<int>(command::cmd_add_friend_response),std::bind(&chatHandler::friend_add_respond,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    msgHandlers.insert({static_cast<int>(command::cmd_friend_list),
        std::bind(&chatHandler::refresh_friend_list,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    

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
        std::cout<<"校验成功！"<<std::endl;
        
        userMap.insert({acc,conn});
        Json::Value res;
        res["cmd"] =  static_cast<int>(command::cmd_login);
        res["res"] = "yes";
        res["nickname"] = usr.getNickName();
        res["icon"] = usr.getIcon();
        res["state"] = "online";

        //修改在线消息

        //获取离线消息

        
        //获取群组消息
        
        sendMsgWithHead(conn,res);
        usrMd.updateState("online",acc);
        
    }else{
        std::cout<<"校验失败！"<<std::endl;
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
    std::string account = msg["cmd"].asString();
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
    std::string toAcc = msg["account_receiver"].asString();
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
    std::string receiver = msg["account_receiver"].asString();
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

        std::string state = rec.getState();
        if(1){
            
            Json::Value _res;
            _res["cmd"] =  static_cast<int>(command::cmd_add_friend_request);
            _res["mode"] = "request";
            _res["account_sender"] = sender; 
            _res["nickname_sender"] = nickname_sender; 
            _res["account_receiver"] = rec.getAccount();
            _res["beizhu"] = msg["beizhu"].asString();
            
            auto it =userMap.find(rec.getAccount());

            if(it!=userMap.end())
                sendMsgWithHead(it->second,_res);
            else std::cout<<"好友不在线"<<std::endl;
        }else{
            //离线状态，将消息存在数据库中
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