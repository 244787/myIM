#include"MessageQueue.h"

bool MessageQueue::push(const string& msg){
    try{
        std::unique_lock<std::mutex> _locker(_mtx);

        _queue.push(std::make_shared<std::string>(msg));
        //_cv.notify_one();
        return true;

    }catch(std::exception e){
        cout<<"err occur ,"<<e.what()<<std::endl;
        return false;

    }
}

std::shared_ptr<std::string> MessageQueue::pool(){
    std::shared_ptr<std::string> res;
    std::unique_lock<std::mutex> locker(_mtx);
    if(_queue.size()<=0){
        return res;
    }
    res = _queue.front();
    _queue.pop();
    return res;
}
std::size_t MessageQueue::size(){
    std::unique_lock<std::mutex> _locker(_mtx);
    return _queue.size();
}