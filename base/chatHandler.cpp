#include"chatHandler.h"
#include"msg.h"
#include"connMysql.h"
#include"UserModel.h"
 chatHandler::chatHandler(){
    msgHandlers.insert({static_cast<int>(command::cmd_login),std::bind(&chatHandler::login,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    msgHandlers.insert({static_cast<int>(command::cmd_regist),std::bind(&chatHandler::regist,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});

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
        Json::Value res;
        res["cmd"] =  static_cast<int>(command::cmd_login);
        res["res"] = "yes";
        std::string JsonRes = res.toStyledString();
        std::cout<<"jsonREs: "<<JsonRes<<std::endl;
        muduo::net::Buffer buf;
        buf.append(JsonRes.c_str() ,JsonRes.size());
        int32_t len = static_cast<int32_t>(JsonRes.size());
        int32_t be32 = muduo::net::sockets::hostToNetwork32(len);
        buf.prepend(&be32, sizeof be32);
        conn->send(&buf);
        
    }else{
        std::cout<<"校验失败！"<<std::endl;
        Json::Value res;
        res["cmd"] =  static_cast<int>(command::cmd_login);
        res["res"] = "no";
        std::string JsonRes = res.toStyledString();
        std::cout<<"jsonREs: "<<JsonRes<<std::endl;
        muduo::net::Buffer buf;
        buf.append(JsonRes.c_str() ,JsonRes.size());
        int32_t len = static_cast<int32_t>(JsonRes.size());
        int32_t be32 = muduo::net::sockets::hostToNetwork32(len);
        buf.prepend(&be32, sizeof be32);
        conn->send(&buf);
        
    }

    //
}
void chatHandler::regist(const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time){
    std::cout<<"开始处理注册逻辑"<<std::endl;
    
}
msgHandlerFnc chatHandler::getHandler(int CmdId){
    auto it =msgHandlers.find(CmdId);
    if(it == msgHandlers.end()){
        return [](const muduo::net::TcpConnectionPtr& conn,Json::Value msg,muduo::Timestamp time){
            std::cout<<"error! there is no msgHandlers!\n";
        };
    }
    return msgHandlers[CmdId];
}