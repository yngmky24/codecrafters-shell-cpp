#include <iostream>
#include <string>
#include <array>
#include <cstdlib>
#include <sstream>
#include <filesystem>
#include <stack>
namespace fs = std::filesystem;

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // List of builtin commands
  const std::array<std::string, 5> builtin {"echo", "exit", "type", "pwd", "cd"};

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
      std::stack<std::string> quotes {};
      std::stringstream ss_word {};
      while(ss >> word) {
        if (word == "\'" && quotes.empty()) { // Opening quote
          quotes.push(word);
        }
        else if (word == "\'" && !quotes.empty()) { // Ending quote
          quotes.pop();
          std::cout << ss_word.str() << std::endl;
          ss_word.clear();
        }
        else if (!quotes.empty()) {
          ss_word << word;
        }
        else if (quotes.empty()) {
          std::cout << word << " ";
        }
      }
      std::cout << std::endl;
    }
    else if (command == "type") {
      bool is_builtin = false;
      std::string commandToFind;
      ss >> commandToFind;
      for(int index = 0; index < builtin.size(); index++) {
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
          fs::path p{path};
          fs::path fullPath = p / commandToFind;
          fs::perms permission {fs::status(fullPath).permissions()};
          if(fs::exists(fullPath) && (permission & fs::perms::group_exec) != fs::perms::none) {
            std::cout << commandToFind << " is " << fullPath.string() << std::endl;
            is_builtin = true;
            break;
          }
        }
        if (!is_builtin) {
          std::cerr << commandToFind << ": not found" << std::endl;
        }
      }
    }
    else if (command == "pwd") {
      // Print working directory
      std::cout << fs::current_path().string() << std::endl;
    }
    else if (command == "cd") {
      std::string destPath {};
      ss >> destPath;
      if (destPath == "~") {
        std::string homeDir {getenv("HOME")};
        fs::current_path(homeDir);
      }
      else {
        fs::path p {destPath};
        // If directory exists, change to that directory
        if (fs::exists(p)) {
          fs::current_path(p);
        }
        else {
          // If the directory doesn't exist
          std::cerr << "cd: " << destPath << ": No such file or directory" << std::endl;
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
        fs::path p {path};
        fs::path fullPath {p / command};
        fs::perms permission {fs::status(fullPath).permissions()};
        if (fs::exists(fullPath) && (permission & fs::perms::group_exec) != fs::perms::none) {
          // Pass any arguments from the command line
          fs::path commandArgs {ss.str()};
          // Execute the command
          const char* commandToExecute {commandArgs.c_str()};
          std::system(commandToExecute);
          is_executable = true;
          break;
        }
      }

      // If the given command is non-executable return default message.
      if (!is_executable) {
        std::cerr << line << ": command not found" << std::endl;
      }
    }
  }
}
