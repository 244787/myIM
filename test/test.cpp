#include"Singleton.h"
#include"MessageQueue.h"
#include<iostream>
#include<thread>
#include<unistd.h>
void thread1(){
    while(1){
        std::cout<<" 11111111"<<std::endl;
        Singleton<MessageQueue>::GetInstance()->push("aaaaaaaa");
        sleep(4);
    }
}

void thread2(){
  while(1){
      std::cout<<"2222222222"<<std::endl;
      Singleton<MessageQueue>::GetInstance()->pool();
      sleep(3);
  }
}

int main(){
    std::thread t(thread1);
    //t.join();
    std::cout<<"1";
    std::thread tt(thread2);
    std::cout<<"2";
    t.join(),tt.join();
    return 1;
}