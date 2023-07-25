CREATE DATABASE IF NOT EXISTS IMDB CHARACTER SET utf8;
USE IMDB;
CREATE TABLE IF NOT EXISTs User(
    account VARCHAR(10)  PRIMARY KEY ,
    nickname VARCHAR(50)  NOT NULL  UNIQUE,
    password VARCHAR(50)  NOT NULL,
    state ENUM('online', 'offline')  DEFAULT 'offline'
);

CREATE TABLE IF NOT EXISTs Friend(
    userAccount VARCHAR(10)  NOT NULL,
    friendAccount VARCHAR(10)  NOT NULL,
    PRIMARY KEY (userAccount, friendAccount)
);


CREATE TABLE IF NOT EXISTs Allgroups(
    id INT  PRIMARY KEY AUTO_INCREMENT,
    groupName VARCHAR(50)  NOT NULL UNIQUE,
    groupDesc VARCHAR(200)  DEFAULT " "
);


CREATE TABLE IF NOT EXISTs GroupUser(
    groupId INT NOT NULL,
    userAccount VARCHAR(10)   NOT NULL,
    groupPole ENUM('creator', 'normal')  DEFAULT "normal",
    PRIMARY KEY (groupId,userAccount)
);

CREATE TABLE IF NOT EXISTs OfflineMessage(
    userAccount VARCHAR(10)  NOT NULL,
    message VARCHAR(500)  NOT NULL
);

