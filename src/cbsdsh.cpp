#include <iostream>

#include <cbsdng/message.h>
#include <cbsdng/shell/parser.h>
#include <cbsdng/shell/shell.h>
#include <cbsdng/shell/socket.h>


int main(int argc, char **argv)
{
  Parser parser;
  parser.parse(argc, argv);
  Socket socket(parser.socket());
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
    socket >> message;
    while (message.gettype() != -1)
    {
      std::cout << message.getpayload() << std::flush;
      socket >> message;
    }

    data = parser.subcommandName(0);
    if (data == "ls")
    {
      socket.open();
      message.data(0, Type::BHYVE, data);
      socket << message;
      socket >> message;
      while (message.gettype() != -1)
      {
        std::cout << message.getpayload() << std::flush;
        socket >> message;
      }
    }
  }
  return 0;
}
