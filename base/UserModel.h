#ifndef USERMODEL_H
#define USERMODEL_H
#include<string>
#include<mutex>
#include"connMysql.h"

class User{
public:
    User(std::string acc = " ",std::string name = " ",std::string password = " ",
        std::string state = "Offline",std::string icon = ":/src/QQIcon/icon1.jpg"):_account(acc),_nickname(name),_password(password),_state(state),_icon(icon){
        
    }
    User(const User& usr){
        _account=usr.getAccount();
        _password=usr.getPassword();
        _nickname = usr.getNickName();
        _state = usr.getState();
        _icon = usr.getIcon();

    }
    std::string getAccount()const{
        return _account;
    }
    std::string getPassword()const{
        return _password;
    }
    std::string getState()const {
        return _state;
    }
    std::string getNickName() const{
        return _nickname;
    }
    std::string getIcon() const{
        return _icon;
    }
    void setAccount(const std::string acc){
        std::unique_lock<std::mutex> locker(mtx);
        _account = acc;
    }
    void setPassword(std::string pwd){
        std::unique_lock<std::mutex> locker(mtx);
        _password = pwd;
    }
    void setNickName(std::string nn){
        std::unique_lock<std::mutex> locker(mtx);
        _nickname = nn;
    }
    void setState(const std::string& state){
        _state = state;
    }
    void setIcon(const std::string& icon){
        _icon = icon;
    }

private:
    std::string _account;
    std::string _nickname;
    std::string _password;
    std::string _state;
    std::string _icon;
    std::mutex mtx;

};

class UserModel{
public:
    bool insert(const User& usr){
        if(!connMysql::GetInstance()->MySQLIsConnected()){
            std::cout<<"MySQL 离线!\n";
            return false;
        }
        char  sql[1024];
        string acc = usr.getAccount();
        string pwd = usr.getPassword();
        string nn = usr.getNickName();
        std::string state = usr.getState();
        sprintf(sql,"insert into User(account,nickname,password,state) values(\"%s\",\"%s\",\"%s\",\"%s\");",acc.c_str(),nn.c_str(),pwd.c_str(),state.c_str());
        std::cout<<"sql:  "<<sql<<std::endl;
        return  connMysql::GetInstance()->Update(sql);
        //return true;
    }
    User query(std::string _account){
        if(!connMysql::GetInstance()->MySQLIsConnected()){
            std::cout<<"MySQL 离线!\n";
            return User();
        }
        string sql;
        sql ="select * from User where account = \""+_account+"\";";
        std::cout<<"sql:  "<<sql<<std::endl;
        if(connMysql::GetInstance()->Query(sql)){
            std::cout<<"find!"<<std::endl;
            MYSQL_RES *res = connMysql::GetInstance()->getResult();
            if (res != nullptr)
            {
                MYSQL_ROW row = mysql_fetch_row(res);
                if (row != nullptr)
                {
                    std::cout<<"res:"<<row[0]<<row[1]<<row[2]<<row[3]<<row[4]<<std::endl;
                    User user(row[0],row[1],row[2],row[3],row[4]);
                    connMysql::GetInstance()->freeResult();

                    return user;
                }

            }

        }
        return User();
    }
    bool updateState(std::string state,std::string account){
        if(!connMysql::GetInstance()->MySQLIsConnected()){
            std::cout<<"MySQL 离线!\n";
            return false;
        }
        char  sql[1024];
        sprintf(sql,"update set User \"state\"=\"%s\" where \"account\" =\"%s\";",state.c_str(),account.c_str());
        std::cout<<"sql:  "<<sql<<std::endl;
        return  connMysql::GetInstance()->Update(sql);  
    }
};


#endif