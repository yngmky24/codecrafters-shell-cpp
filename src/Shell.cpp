#include "../include/Shell.hpp"
#include "../include/Builtins.hpp"
#include "../include/Operator.hpp"
#include <cstddef>
#include <iostream>

Shell::Shell() {
  builtins["exit"] = std::make_unique<Exit>();
  builtins["echo"] = std::make_unique<Echo>();
  builtins["type"] = std::make_unique<Type>();
  builtins["pwd"]  = std::make_unique<Pwd>();
  builtins["cd"]   = std::make_unique<Cd>();
}

void Shell::run() {
  std::string line {};
  std::string command {};
  // Loop infinitely until user calls `exit` command.
  while(1) {
    std::cout << "$ ";
    std::getline(std::cin, line);
  
    // Get the parsed tokens
    std::vector<std::string> tokens = parser.parseLine(line);
    if (tokens.empty()) continue;
    command = tokens[0];
    
    Operator op(tokens);

    // Execute builtin command
    if(builtins.find(command) != builtins.end()) {
      builtins[command]->execute(tokens);
    }
    else {
      Undefined undefinedCmd;
      undefinedCmd.execute(tokens);
    }
  }
}
