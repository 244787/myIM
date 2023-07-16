#ifndef CONNMYSQL_H
#define CONNMYSQL_H
#include<string>
#include<iostream>
#include<mysql/mysql.h>
#include<chrono>
#include<mutex>
#include"Singleton.h"
class connMysql :public Singleton<connMysql> {
    friend class Singleton<connMysql>;
public:
    ~connMysql();
    bool Connect(const std::string& ip,
        const std::string& user,
        const std::string& pwd,
        const std::string& db,
        const unsigned short& port);
    bool Update(const std::string& sql);
    bool Query(const std::string& sql);
    bool MySQLIsConnected(){return isConnected;}
    MYSQL_RES* getResult();
    int affectRows();
    void freeResult();
private:
    connMysql();
    void init();
    void uninit();
private:
    bool isConnected;
    MYSQL* m_mysql;
    MYSQL_RES* m_result;
    MYSQL_ROW m_row = nullptr;
    std::mutex mtx;

};

#endif