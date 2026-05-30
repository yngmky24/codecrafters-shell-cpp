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
  std::string line {};
  std::string command {};
  // Loop infinitely until user calls `exit` command.
  while(1) {
    std::cout << "$ ";
    std::getline(std::cin, line);
    std::stringstream ss {line};
    ss >> command;
    if (command == "exit") {
      exit(0);
    }
    else if (command == "echo") {
      std::string word {};
      while(ss >> word) {
        std::cout << word << " ";
      }
      std::cout << std::endl;
    }
    else if (command == "type") {
      bool is_builtin = false;
      // List of builtin commands
      std::string builtin[3] {"echo", "exit", "type"};
      std::string commandToFind;
      ss >> commandToFind;
      for(int index = 0; index < builtin->length(); index++) {
        if (commandToFind == builtin[index]) {
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
          std::filesystem::path fullPath = p / commandToFind;
          std::filesystem::perms permission {std::filesystem::status(fullPath).permissions()};
          if(std::filesystem::exists(fullPath) && (permission & std::filesystem::perms::group_exec) != std::filesystem::perms::none) {
            std::cout << commandToFind << " is " << fullPath.string() << std::endl;
            is_builtin = true;
            break;
          }
        }
        if (!is_builtin) {
          std::cout << commandToFind << ": not found" << std::endl;
        }
      }
    }
    else {
      bool is_executable {false};
      // Determine if the given command is an executable
      std::string path_env = std::getenv("PATH");
      std::stringstream ss_path(path_env);
      std::string path;
      while(std::getline(ss_path, path, ':')) {
        std::filesystem::path p{path};
        std::filesystem::path fullPath = p / command;
        std::filesystem::perms permission {std::filesystem::status(fullPath).permissions()};
        if (std::filesystem::exists(fullPath) && (permission & std::filesystem::perms::group_exec) != std::filesystem::perms::none) {
          std::cout << fullPath.string() << std::endl; 
          // Pass any arguments from the command line
          std::string word {};
          std::stringstream ss_args {fullPath};
          while(ss >> word) {
            ss_args << " " << word;
          }
          // Execute the command
          std::cout << "Full command: " << ss_args.str() << std::endl;
          is_executable = true;
          break;
        }
      }

      // If the given command is non-executable return default message.
      if (!is_executable) {
        std::cout << line << ": command not found" << std::endl;
      }
    }
  }
}
