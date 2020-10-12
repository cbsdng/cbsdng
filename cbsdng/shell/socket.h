#pragma once

#include <cbsdng/message.h>
#include <cbsdng/shell/socket.h>

#include <string>

class Socket
{
public:
  Socket(const std::string &socketPath);
  ~Socket();

  bool write(const std::string &data);

protected:
  int fd;
  std::string socketPath;
};

Socket &operator<<(Socket &sock, const Message &message);
Socket &operator>>(Socket &sock, Message &message);
