#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/event.h>
#include <termios.h>
#include <unistd.h>

#include "cbsdng/shell/socket.h"


Socket::Socket(const std::string &socket_path)
  : socketPath{socket_path}
  , fd{-1}
  , kq{-1}
  , remaining{0}
{
  events = new struct kevent[2];
  targetEvent = new struct kevent;
  kq = kqueue();
  if (kq == -1)
  {
    std::stringstream error;
    error << "kqueue: " << strerror(errno) << '\n';
    cleanup();
    throw error.str();
  }
  EV_SET(events, STDIN_FILENO, EVFILT_READ, EV_ADD | EV_CLEAR, NOTE_READ, 0, nullptr);
  open();
}


void Socket::cleanup()
{
  if (fd != -1)
  {
    close(fd);
    fd = -1;
  }
  EV_SET(events + 1, fd, EVFILT_READ, EV_DELETE | EV_CLEAR, NOTE_READ, 0, nullptr);
}


void Socket::open()
{
  struct sockaddr_un addr;
  if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
  {
    std::cerr << "socket error: " << strerror(errno) << '\n';
    exit(1);
  }
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socketPath.data(), socketPath.size());
  if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
  {
    std::cerr << "Connecting to " << socketPath << " failed: ";
    std::cerr << strerror(errno) << '\n';
    exit(1);
  }
  EV_SET(events + 1, fd, EVFILT_READ, EV_ADD | EV_CLEAR, NOTE_READ, 0, nullptr);
  int ret = kevent(kq, events, 2, nullptr, 0, nullptr);
  if (ret == -1)
  {
    std::stringstream error;
    error << "kevent register: " << strerror(errno) << '\n';
    cleanup();
    throw error.str();
  }
}


const Message Socket::read()
{
  int ret;
  if (remaining > 0)
  {
    auto m = readMeta();
    readSocket(m);
    return m;
  }
  remaining = 0;
  while (true)
  {
    ret = kevent(kq, nullptr, 0, targetEvent, 1, nullptr);
    if (targetEvent->ident == STDIN_FILENO)
    {
      char *input = new char[targetEvent->data+1];
      ret = ::read(targetEvent->ident, input, targetEvent->data);
      if (ret > 0)
      {
        input[ret] = '\0';
        Message message(0, 0, input);
        delete []input;
        (*this) << message;
      }
      continue;
    }
    else
    {
      if (ret == -1 || targetEvent->data == 0)
      {
        if (errno == 0) { return Message(0, Type::CONNECTION_CLOSED, ""); }
        return Message(0, -1, strerror(errno));
      }
      remaining = targetEvent->data;
      Message m = readMeta();
      if (!readSocket(m))
      {
        m.type(-1);
      }
      return m;
    }
  }
}


const Message Socket::message(char const * const buffer)
{
  if (buffer == nullptr)
  {
    return Message(0, -1, strerror(errno));
  }
  std::stringstream sstr;
  sstr << buffer;
  int id;
  int type;
  std::string data;
  sstr >> id >> type;
  size_t pos = sstr.tellg();
  sstr.seekg(pos + 1);
  getline(sstr, data, '\0');
  Message message(0, 0, data);
  return message;
}


Message Socket::readMeta()
{
  Message m;
  uint32_t data;
  int rc = ::read(fd, &data, sizeof(data));
  if (rc <= 0)
  {
    remaining = 0;
    m.type(-1);
    m.payload(strerror(errno));
    return m;
  }
  m.id(data);
  remaining -= rc;

  rc = ::read(fd, &data, sizeof(data));
  if (rc <= 0)
  {
    remaining = 0;
    m.type(-1);
    m.payload(strerror(errno));
    return m;
  }
  m.type(data);
  remaining -= rc;

  rc = ::read(fd, &data, sizeof(data));
  if (rc <= 0)
  {
    remaining = 0;
    m.type(-1);
    m.payload(strerror(errno));
    return m;
  }
  m.resize(data);
  remaining -= rc;
  return m;
}

bool Socket::readSocket(Message &m)
{
  int rc;
  std::string payload;
  int total = m.payload().size();
  payload.resize(total);
  while (total > 0)
  {
    rc = ::read(fd, (char *)payload.data(), total);
    if (rc <= 0)
    {
      remaining = 0;
      return false;
    }
    total -= rc;
    if (remaining > 0) { remaining -= rc; }
  }
  m.payload(payload);
  return true;
}


bool Socket::write(const uint32_t &size)
{
  auto sentSize = ::write(fd, &size, sizeof(size));
  if (sentSize <= 0)
  {
    std::cerr << "Error: " << strerror(errno) << '\n';
    return false;
  }
  return true;
}


bool Socket::write(const std::string &data)
{
  auto sentSize = ::write(fd, data.data(), data.size());
  if (sentSize <= 0)
  {
    std::cerr << "Error: " << strerror(errno) << '\n';
    return false;
  }
  return true;
}


Socket &operator<<(Socket &sock, const Message &message)
{
  sock.write(message.id());
  sock.write(message.type());
  const auto &payload= message.payload();
  sock.write(payload.size());
  sock.write(payload);
  return sock;
}


Socket &operator>>(Socket &sock, Message &message)
{
  message = sock.read();
  return sock;
}


Socket::~Socket()
{
  cleanup();
  delete []events;
  delete targetEvent;
}
