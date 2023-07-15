#ifndef USERMODEL_H
#define USERMODEL_H
#include<string>

#include"connMysql.h"
enum class UserState{
    Online = 0,
    Offline 
} ;

class User{
public:
    User(std::string acc = " ",std::string password = " ",std::string name = " ",std::string state =" "):_account(acc),_nickname(name),_password(password),_state(state){
        
    }
    std::string getAccount(){
        return _account;
    }
    std::string getName(){
        return _nickname;
    }
    std::string getPassword(){
        return _password;
    }
    std::string getState(){
        return _state;
    }
    

private:
    std::string _account;
    std::string _nickname;
    std::string _password;
    std::string _state;

};

class UserModel{
public:
    bool insert(const User& ){

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
                    std::cout<<"res:"<<row[0]<<row[1]<<std::endl;
                    User user(row[0],row[1],"yes","ok");
                    //User user(row[0],row[1],row[2],row[3]);
                    connMysql::GetInstance()->freeResult();
                    //释放资源，否则内存不断泄露
                    //mysql_free_result(res);

                    return user;
                }

            }

        }
        return User();
    }
};


#endif