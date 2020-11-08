#include <iostream>
#include <sstream>
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
    Message command(0, 0, data);
    socket << command;
    int rc;
    Message output;
    while (true)
    {
      socket >> output;
      if (output.type() == -1)
      {
        std::cout << "Proxy error: " << output.payload() << std::endl;
        break;
      }
      if (output.type() == Type::EXIT)
      {
        std::stringstream s;
        s << output.payload();
        s >> rc;
        if (rc > 0) { return rc; }
        break;
      }
      std::cout << output.payload() << std::flush;
    }

    if (parser.subcommandName(0) == "ls")
    {
      socket.open();
      command.type(Type::BHYVE);
      socket << command;
      while (true)
      {
        socket >> output;
        if (output.type() == -1)
        {
          std::cout << "Proxy error: " << output.payload() << std::endl;
          std::cout << output.payload() << std::endl;
          break;
        }
        if (output.type() == Type::EXIT)
        {
          std::stringstream s;
          s << output.payload();
          s >> rc;
          if (rc > 0) { return rc; }
          break;
        }
        std::cout << output.payload() << std::flush;
      }
    }
  }
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTerm);
  return 0;
}
