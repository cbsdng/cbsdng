#pragma once

#include <cbsdng/message.h>
#include <cbsdng/shell/socket.h>

#include <string>

class Socket
{
public:
  Socket(const std::string &socketPath);
  ~Socket();

  const Message read(size_t size = 1024);
  bool write(const std::string &data);

protected:
  int fd;
  std::string socketPath;
};

Socket &operator<<(Socket &sock, const Message &message);
Socket &operator>>(Socket &sock, Message &message);
