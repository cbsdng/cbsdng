#pragma once

#include <cbsdng/message.h>
#include <cbsdng/shell/socket.h>

#include <string>

class Socket
{
public:
  Socket(const std::string &socketPath);
  ~Socket();

  void open();
  void cleanup();
  const Message read(size_t size = 1024);
  bool write(const std::string &data);

protected:
  int fd;
  int kq;
  std::string socketPath;
  struct kevent *events;
  struct kevent *targetEvent;
};

Socket &operator<<(Socket &sock, const Message &message);
Socket &operator>>(Socket &sock, Message &message);
