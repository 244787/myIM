//消息队列,将收到的一个完整消息放入队列，等待处理线程去除消息进行消费
#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_
#include<memory>
#include<mutex>
#include<string>
#include"Singleton.h"
#include<queue>
//#include"json/json.h"
#include<condition_variable>
class MessageQueue : public Singleton<MessageQueue>{
    friend class Singleton<MessageQueue>;
    public:
        ~MessageQueue() = default;
    public:
        MessageQueue() = default;
        MessageQueue(const MessageQueue&) = delete;
        MessageQueue operator=(const MessageQueue& ) = delete;
        bool push(const std::string& msg);
        std::shared_ptr<std::string>pool();
        std::size_t size();
        


    private:
        std::queue<std::shared_ptr<std::string>> _queue;
        std::mutex _mtx;
        //std::condition_variable _cv;


        

        
};

#endif