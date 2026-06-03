#pragma once
#include <vector>
#include <string>

enum class ParseState {
  Normal,
  // NormalAfterSpace,
  InSingleQuote,
  InDoubleQuote,
  Escaped
};

class Parser {
public:
  std::vector<std::string> parseLine(const std::string& line);
};
