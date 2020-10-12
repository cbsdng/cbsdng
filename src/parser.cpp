#include <cbsdng/shell/parser.h>

#include <cbsdng/shell/CLI/CLI.hpp>

static CLI::App app("CBSD shell");

Parser::Parser()
{
  app.set_help_all_flag("--help-all", "Expand all help");
  auto construct = app.add_subcommand("construct", "Construct resource");
  construct->add_option("jail", _jails, "Jail name")->required();
  auto start = app.add_subcommand("start", "Start resource");
  start->add_option("jail", _jails, "Jail name")->required();
  auto stop = app.add_subcommand("stop", "Stop resource");
  stop->add_option("jail", _jails, "Jail name")->required();
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

std::map<std::string, std::string> Parser::options() { return _options; }

std::vector<std::string> Parser::jails() { return _jails; }


std::string Parser::subcommandName(const int &index)
{
  return app.get_subcommands()[index]->get_name();
}


int Parser::subcommandsSize()
{
  return app.get_subcommands().size();
}
