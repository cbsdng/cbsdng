#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cbsdng/shell/socket.h>


Socket::Socket(const std::string &socket_path) : socketPath{socket_path}
{
  struct sockaddr_un addr;

  if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
  {
    perror("socket error");
    exit(-1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socketPath.data(), sizeof(addr.sun_path) - 1);

  if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
  {
    std::cerr << "Connecting to " << socketPath << " failed: ";
    std::cerr << strerror(errno) << std::endl;
    exit(-1);
  }
}


Socket::~Socket() { close(fd); }


bool Socket::write(const std::string &data)
{
  std::stringstream s;
  s << data.size() << ' ' << data;
  auto sentSize = send(fd, s.str().data(), s.str().size(), MSG_NOSIGNAL);
  if (sentSize < 0)
  {
    std::cerr << "Error: " << strerror(errno) << std::endl;
    return false;
  }
  else if (sentSize == 0)
  {
    std::cerr << "Error: socket closed" << std::endl;
    return false;
  }
  // char buffer[1024];
  // auto receivedSize = recv(fd, buffer, sizeof(buffer), MSG_NOSIGNAL);
  // if (receivedSize < 0)
  // {
  // std::cerr << "Error: " << strerror(errno) << std::endl;
  // return false;
  // }
  // else if (receivedSize == 0)
  // {
  // std::cerr << "Error: socket closed" << std::endl;
  // return false;
  // }
  return true;
}


Socket &operator<<(Socket &sock, const Message &message)
{
  sock.write(message.data());
  return sock;
}


Socket &operator>>(Socket &sock, Message &message)
{
  message.data(5, 8, "random");
  return sock;
}
