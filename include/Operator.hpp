#pragma once
#include <vector>
#include <string>


class Operator {
private:
  const std::vector<std::string> OperatorSymbols {"1>", ">"};
  int saved_stdout = -1;
  int target_fd = -1;
public:
  /*Constructor*/ Operator(std::vector<std::string>& args);
  /*Destructor*/ ~Operator();
  void redirect(std::string targetFile);
};
