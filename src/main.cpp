#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

// List of builtin commands
std::vector<std::string> builtin {
  "echo",
  "exit",
  "type"
};

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::string userInput {};
  while(1) {
    std::cout << "$ ";
    std::getline(std::cin, userInput);
    if (userInput == "exit") {
      exit(0);
    }
    else if (userInput.substr(0, 5) == "echo ") {
      std::cout << userInput.substr(5) << '\n';
    }
    else if (userInput.substr(0, 5) == "type ") {
      bool is_builtin = false;
      for(int index = 0; index < builtin.size(); index++) {
        if (userInput.substr(5) == builtin[index]) {
          std::cout << builtin[index] << " is a shell builtin\n";
          is_builtin = true;
          break;
        }
      }
      if (!is_builtin) {
        std::cout << userInput.substr(5) << ": not found\n";
      }
    }
    else {
      std::cout << userInput << ": command not found\n";
    }
  }
}
