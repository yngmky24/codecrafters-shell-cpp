#include <iostream>
#include <string>
#include <array>
#include <cstdlib>
#include <sstream>
#include <filesystem>
namespace fs = std::filesystem;

enum class ParseState {
  Normal,
  NormalAfterSpace,
  InSingleQuote
};

class Command {

};

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
    ss >> std::ws; // consume whitespace after command
    if (command == "exit") {
      exit(0);
    }
    else if (command == "echo") {      
      ParseState state {ParseState::Normal};
      char c {};
      std::stringstream ss_word {};
      while(ss >> std::noskipws >> c) {
        if (c == '\'' && (state==ParseState::Normal || state==ParseState::NormalAfterSpace)) { // Opening quote
          state = ParseState::InSingleQuote;
        }
        else if (c == '\'' && state==ParseState::InSingleQuote) { // Ending quote          
          state = ParseState::Normal;       
          std::cout << ss_word.str();
          ss_word.str(""); // empties the text
          ss_word.clear();
        }
        else if (state==ParseState::InSingleQuote) { // Between single quotes
          ss_word << c;
        }
        else if (state==ParseState::Normal) {
          if (c == ' ') {
            state = ParseState::NormalAfterSpace;
            std::cout << ' ';
          }
          else {            
            std::cout << c;          
          }
        }
        else if (state==ParseState::NormalAfterSpace) {
          if (c != ' ') {
            state = ParseState::Normal;
            std::cout << c;
          }
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
          std::string fullPathArgs {fullPath.string()};
          ss >> fullPathArgs;          
          // fs::path commandArgs {ss.str()};
          fs::path commandArgs {fullPathArgs};
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
