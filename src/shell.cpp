#include <iostream>
#include <vector>
#include <replxx.hxx>

#include "cbsdng/shell/shell.h"


Shell::Shell(Parser &p, Socket &s) : parser{p}, socket{s} {}


void Shell::run()
{
  std::cout << "Welcome to CBSDng interactive shell\n";
  replxx::Replxx rx;
  rx.install_window_change_handler();
  char *raw_home = getenv("HOME");
  std::string home;
  if (!raw_home)
  {
    home = "";
  }
  else
  {
    home = raw_home;
  }
  std::string history_file = home + "/.cbsdsh_history";
  rx.history_load(history_file);
  while (true)
  {
    auto raw_input = rx.input("> ");
    if (raw_input == nullptr)
    {
      if (errno == EAGAIN) { continue; }
      else { break; }
    }
    std::stringstream sstream;
    std::vector<std::string> args;
    bool ok = true;
    bool help = false;
    sstream << raw_input;
    while (!sstream.eof())
    {
      std::string s;
      sstream >> s;
      if (sstream.fail())
      {
        ok = false;
        break;
      }
      if (s == "-h" or s == "--help" or s == "--help-all") { help = true; }
      args.insert(args.begin(), s);
    }
    if (help) { continue; }
    if (ok)
    {
      auto rc = parser.parse(args);
      if (rc == 0)
      {
        rx.history_add(raw_input);
        if (parser.subcommandsSize() == 0)
        {
          continue;
        }
        if (help)
        {
          continue;
        }
        std::string data = parser.subcommandName(0);
        for (auto jail : parser.jails())
        {
          data += ' ';
          data += jail;
        }
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
            return;
          }
          if (output.type() == Type::EXIT)
          {
            std::stringstream s;
            s << output.payload();
            s >> rc;
            if (rc > 0) { return; }
            break;
          }
          if (output.type() == Type::CONNECTION_CLOSED) { break; }
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
              return;
            }
            if (output.type() == Type::EXIT)
            {
              std::stringstream s;
              s << output.payload();
              s >> rc;
              if (rc > 0) { return; }
              break;
            }
            std::cout << output.payload() << std::flush;
          }
        }
        socket.open();
      }
    }
  }
  rx.history_save(history_file);
}
