#pragma once
#include "../include/Parser.hpp"
#include "../include/Command.hpp"
#include <string>
#include <unordered_map>
#include <memory>

class Shell {
private:
  const std::string PATH = "PATH";
  const std::string HOME = "HOME";
  Parser parser;
  std::unordered_map<std::string, std::unique_ptr<Command>> builtins {};

public:
  Shell();
  void run();
};
