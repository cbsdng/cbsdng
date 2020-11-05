#pragma once

#include <string>

#include <cbsdng/message.h>
#include "cbsdng/shell/socket.h"


class Socket
{
public:
  Socket(const std::string &socketPath);
  ~Socket();

  void open();
  void cleanup();
  const Message read();
  Message readMeta();
  bool readSocket(Message &m);
  bool write(const std::string &data);
  bool write(const uint32_t &size);
  const Message message(char const * const buffer);

protected:
  int fd;
  int kq;
  int remaining;
  std::string socketPath;
  struct kevent *events;
  struct kevent *targetEvent;
};

Socket &operator<<(Socket &sock, const Message &message);
Socket &operator>>(Socket &sock, Message &message);
