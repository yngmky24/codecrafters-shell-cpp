#pragma once
#include <vector>
#include <string>

class Parser {
private:
  enum class ParseState {
    Normal,
    NormalAfterSpace,
    InSingleQuote,
    InDoubleQuote
  };
public:
  std::vector<std::string> parseLine(const std::string& line);
};
