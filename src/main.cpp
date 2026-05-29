#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <filesystem>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Initialize an object to hold user inputs.
  std::string userInput {};
  // Loop infinitely until user calls `exit` command.
  while(1) {
    std::cout << "$ ";
    std::getline(std::cin, userInput);
    if (userInput == "exit") {
      exit(0);
    }
    else if (userInput.substr(0, 5) == "echo ") {
      std::cout << userInput.substr(5) << std::endl;
    }
    else if (userInput.substr(0, 5) == "type ") {
      bool is_builtin = false;
      // List of builtin commands
      std::string builtin[3] {"echo", "exit", "type"};
      for(int index = 0; index < builtin->length(); index++) {
        if (userInput.substr(5) == builtin[index]) {
          std::cout << builtin[index] << " is a shell builtin" << std::endl;
          is_builtin = true;
          break;
        }
      }
      if (!is_builtin) {
        std::string path_env = std::getenv("PATH");
        std::stringstream ss_path(path_env);
        std::string path;
        while(std::getline(ss_path, path, ':')) {
          std::filesystem::path p{path};
          std::filesystem::path fullPath = p / userInput.substr(5);
          if(std::filesystem::exists(fullPath)) {
            std::cout << userInput.substr(5) << " is " << fullPath;
            is_builtin = true;
            break;
          }
        }
        std::cout << userInput.substr(5) << ": not found" << std::endl;
      }
    }
    else {
      std::cout << userInput << ": command not found" << std::endl;
    }
  }
}
