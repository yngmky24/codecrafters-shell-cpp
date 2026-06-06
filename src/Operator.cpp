#include "../include/Operator.hpp"
#include <unistd.h>
#include <fcntl.h>
Operator::Operator(std::vector<std::string>& args) {
  // Scan for operator
  for (size_t i = 1; i < args.size(); i++) {
    for (const auto s : OperatorSymbols) {
      if (args[i] == "1>" || args[i] == ">") {   
        redirect(args[i+1]);       
        // Clean arguments (remove all token from and after the operator)
        args.resize(i-1);
        break;
      }
    }
  }
}
Operator::~Operator() {
  if (saved_stdout != -1) {
    dup2(saved_stdout, 1);
    close(saved_stdout);
  }
  if (target_fd != 1) {
    close(target_fd);
  }
}

void Operator::redirect(std::string targetFile) {
  // Save the original standard output
  saved_stdout = dup(1);

  // Open the target file (create if it does not exists)
  target_fd = open(targetFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);

  // Redirect standard output to the target file
  if (target_fd != -1) {
    dup2(target_fd, 1);
  }
}
