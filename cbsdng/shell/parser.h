#pragma once

#include <map>
#include <string>
#include <vector>

class Parser
{
public:
  Parser();
  ~Parser();

  int parse(const int &argc, const char * const *argv);
  int parse(std::vector<std::string> &argv);

  std::map<std::string, std::string> options();
  std::vector<std::string> jails();
  std::string subcommandName(const int &index);
  int subcommandsSize();

protected:
  std::map<std::string, std::string> _options;
  std::vector<std::string> _jails;
};
