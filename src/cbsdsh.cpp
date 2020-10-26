#include <iostream>
#include <termios.h>
#include <unistd.h>

#include <cbsdng/message.h>
#include "cbsdng/shell/parser.h"
#include "cbsdng/shell/shell.h"
#include "cbsdng/shell/socket.h"


int main(int argc, char **argv)
{
  Parser parser;
  parser.parse(argc, argv);
  Socket socket(parser.socket());
  struct termios origTerm;
  tcgetattr(STDIN_FILENO, &origTerm);
  struct termios newTerm = origTerm;
  newTerm.c_iflag &= ~(BRKINT | INPCK | ISTRIP | IXON);
  newTerm.c_cflag |= (CS8);
  newTerm.c_lflag &= ~(ECHO | ECHONL | ECHOE | ECHOKE | ECHOCTL | ECHOPRT | ICANON | IEXTEN | ISIG);
  newTerm.c_cc[VMIN] = 1;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &newTerm);
  if (parser.subcommandsSize() == 0)
  {
    Shell shell(parser, socket);
    shell.run();
  }
  else
  {
    std::string data = parser.subcommandName(0);
    for (auto jail : parser.jails())
    {
      data += ' ';
      data += jail;
    }
    data += ' ';
    data += parser.jail();
    Message message;
    message.data(0, 0, data);
    socket << message;
    while (true)
    {
      socket >> message;
      if (message.type() == -1) { break; }
      std::cout << message.payload() << std::flush;
    }

    data = parser.subcommandName(0);
    if (data == "ls")
    {
      socket.open();
      message.data(0, Type::BHYVE, data);
      socket << message;
      while (true)
      {
        socket >> message;
        if (message.type() == -1) { break; }
        std::cout << message.payload() << std::flush;
      }
    }
  }
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTerm);
  return 0;
}
