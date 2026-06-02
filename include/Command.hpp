#pragma once
#include <string>
#include <vector>

class Command {
public:
  // Virtual destructor for base classes
  virtual ~Command() = default;
  // Every specific command will implement this differently
  virtual void execute(const std::vector<std::string>& args) = 0;
};
