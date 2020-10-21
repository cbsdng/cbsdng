#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/event.h>
#include <unistd.h>

#include <cbsdng/shell/socket.h>


Socket::Socket(const std::string &socket_path)
  : socketPath{socket_path}
  , fd{-1}
{
  open();
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
}


const Message Socket::read(size_t size)
{
  auto event = new struct kevent;
  auto tevent = new struct kevent;
  int kq = kqueue();
  if (kq == -1) { std::cerr << "kqueue: " << strerror(errno) << '\n'; }
  EV_SET(event, fd, EVFILT_READ, EV_ADD | EV_CLEAR, NOTE_READ, 0, nullptr);
  int ret = kevent(kq, event, 2, nullptr, 0, nullptr);
  if (ret == -1)
  {
    std::cerr << "kevent register: " << strerror(errno) << '\n';
    if (event->flags & EV_ERROR)
    {
      std::cerr << "kevent register: " << strerror(event->data) << '\n';
    }
    return Message(0, -1, "");
  }
  ret = kevent(kq, nullptr, 0, tevent, 1, nullptr);
  if (ret == -1 || tevent->data == 0) { return Message(0, -1, ""); }
  char *buffer = new char[tevent->data+1];
  ret = ::read(tevent->ident, buffer, tevent->data);
  if (ret <= 0) { return Message(0, -1, ""); }
  buffer[ret] = '\0';
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
  delete []buffer;
  delete event;
  delete tevent;
  return message;
}


bool Socket::write(const std::string &data)
{
  auto sentSize = ::write(fd, data.data(), data.size());
  if (sentSize < 0)
  {
    std::cerr << "Error: " << strerror(errno) << '\n';
    return false;
  }
  else if (sentSize == 0)
  {
    std::cerr << "Error: socket closed!\n";
    return false;
  }
  return true;
}


Socket &operator<<(Socket &sock, const Message &message)
{
  sock.write(message.data());
  return sock;
}


Socket &operator>>(Socket &sock, Message &message)
{
  message = sock.read();
  return sock;
}


Socket::~Socket()
{
  if (fd != -1)
  {
    close(fd);
    fd = -1;
  }
}
