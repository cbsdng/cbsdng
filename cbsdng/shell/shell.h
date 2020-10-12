#pragma once

#include "parser.h"
#include "socket.h"

class Shell
{
public:
  Shell(Parser &p, Socket &s);

  void run();

protected:
  Parser &parser;
  Socket &socket;
};
