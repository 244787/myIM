#ifndef FRIENDMODEL_H_
#define FRIENDMODEL_H_
#include"json/json.h"
#include"connMysql.h"
#include"UserModel.h"
class FriendModel{
public:
    bool insert(const string& usrAcc,const string& friendAcc){
        char sql[2014]={};
        sprintf(sql,"insert into Friend VALUE(\"%s\",\"%s\");",usrAcc.c_str(),friendAcc.c_str());
        std::cout<<"加好友操作，sql:"<<sql<<std::endl;
        if(!connMysql::GetInstance()->MySQLIsConnected())    return false;
        return connMysql::GetInstance()->Query(std::string(sql));
    }
    //返回好友列表
    std::vector<User> query(const std::string &userAccount){
        char sql[1024]={};
        sprintf(sql,"select a.account,a.nickname,a.state, a.icon from User a inner join Friend b on a.account = b.friendAccount where b.userAccount =\"%s\";",userAccount.c_str());
        std::cout<<sql<<std::endl;
        if(!connMysql::GetInstance()->Query(sql)){
            return std::vector<User>();
        }
        MYSQL_RES *res=connMysql::GetInstance()->getResult();
        if(res!=nullptr){
            vector<User> vec;
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr){
                User user;
                user.setAccount(row[0]);
                user.setNickName(row[1]);
                user.setState(row[2]);
                user.setIcon(row[3]);
                vec.push_back(user);
            }
            //释放资源，否则内存不断泄露
            connMysql::GetInstance()->freeResult();
            return vec;
        }
        return std::vector<User>();
    }

    bool search_friend(const string& acc,vector<User>& usrs){
        char sql[1024]={};
        sprintf(sql,"select account,nickname,state from User where account =\"%s\";",acc.c_str());
        std::cout<<sql<<std::endl;
        if(!connMysql::GetInstance()->Query(sql)){
           return  false;
        }
        MYSQL_RES *res=connMysql::GetInstance()->getResult();
        if(res!=nullptr){
            MYSQL_ROW row;
            User usr;
            if ((row = mysql_fetch_row(res)) != nullptr){
                usr.setAccount(row[0]);
                usr.setNickName(row[1]);
                usr.setState(row[2]);
                usrs.push_back(usr);
            }    
            //释放资源，否则内存不断泄露
            connMysql::GetInstance()->freeResult();
            return true;
        }
        return false;

    }

};


#endif