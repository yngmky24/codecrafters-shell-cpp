#pragma once
#include "Command.hpp"
#include <array>

class Exit : public Command {
public:
  void execute(const std::vector<std::string>&args) override;
};

class Echo : public Command {
public:
  void execute(const std::vector<std::string>&args) override;
};

class Type : public Command {
private:
  // List of builtin commands
  const std::array<std::string, 5> builtin {"echo", "exit", "type", "pwd", "cd"};
public:
  void execute(const std::vector<std::string>&args) override;
};

class Pwd : public Command {
public:
  void execute(const std::vector<std::string>&args) override;
};

class Cd : public Command {
public:
  void execute(const std::vector<std::string>&args) override;
};

class Undefined : public Command {
public:
  void execute(const std::vector<std::string>&args) override;
};
