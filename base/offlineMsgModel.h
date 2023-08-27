#ifndef OFFLINEMSGMODEL_H_
#define OFFLINEMSGMODEL_H_
#include<iostream>
#include"connMysql.h"
#include<string>
#include<vector>
#include<json/json.h>
class offlineMsgModel{
public:
    bool pushMsg(const std::string & account,std::string msg ){
        char sql[2014]={};
        sprintf(sql,"insert into OfflineMessage VALUE(\"%s\",\'%s\');",account.c_str(),msg.c_str());
        std::cout<<"添加离线消息，sql:"<<sql<<std::endl;
        if(!connMysql::GetInstance()->MySQLIsConnected())    return false;
        return connMysql::GetInstance()->Query(std::string(sql));
    }
    vector<std::string> pullMsg(const std::string account){
        char sql[1024]={};
        sprintf(sql,"select message from OfflineMessage where userAccount = \"%s\"; ",account.c_str());
        std::cout<<"接受离线消息，sql语句为："<<sql<<std::endl;
        if(!connMysql::GetInstance()->Query(sql)){
            return vector<std::string>();
        }
        MYSQL_RES *res=connMysql::GetInstance()->getResult();
        if(res!=nullptr){
            vector<std::string> vec;
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr){
                vec.push_back(row[0]);
            }
            //释放资源，否则内存不断泄露
            connMysql::GetInstance()->freeResult();
            return vec;
        }
        return std::vector<std::string>();
    }
};

#endif