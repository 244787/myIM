

#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"

#include <unistd.h>
#include"./../base/TcpSession.h"
int main()
{
  LOG_INFO << "pid = " << getpid();
  muduo::net::EventLoop loop;
  muduo::net::InetAddress listenAddr(2007);
  TcpSession server(&loop,listenAddr,"tcSession");
  server.start();
  loop.loop();
}

