#include <cbsdng/shell/parser.h>
#include <cbsdng/shell/CLI/CLI.hpp>


static CLI::App app("CBSD shell");


Parser::Parser() : _socket{"/var/run/cbsdng/cbsdng.sock"}
{
  app.set_help_all_flag("--help-all", "Expand all help");
  auto construct = app.add_subcommand("construct", "Construct instance");
  construct->add_option("jail", _jails, "Jail name")->required();
  auto start = app.add_subcommand("start", "Start instance");
  start->add_option("jail", _jails, "Jail name")->required();
  auto stop = app.add_subcommand("stop", "Stop instance");
  stop->add_option("jail", _jails, "Jail name")->required();
  app.add_option("-s,--socket", _socket, "Socket to connect to");
  app.add_subcommand("ls", "List instances");
}


Parser::~Parser() {}


int Parser::parse(const int &argc, const char *const *argv)
{
  try
  {
    app.parse(argc, argv);
  }
  catch (const CLI::ParseError &e)
  {
    auto rc = app.exit(e);
    exit(rc);
  }
  return 0;
}


int Parser::parse(std::vector<std::string> &argv)
{
  try
  {
    app.parse(argv);
  }
  catch (const CLI::ParseError &e)
  {
    return app.exit(e);
  }
  return 0;
}


std::string Parser::subcommandName(const int &index)
{
  return app.get_subcommands()[index]->get_name();
}


int Parser::subcommandsSize()
{
  return app.get_subcommands().size();
}


std::map<std::string, std::string> Parser::options() { return _options; }
std::vector<std::string> Parser::jails() { return _jails; }
const std::string & Parser::socket() const { return _socket; }
