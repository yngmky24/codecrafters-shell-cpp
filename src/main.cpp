#include <iostream>
#include <string>
#include <array>
#include <vector>
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
public:
  // Virtual destructor for base classes
  virtual ~Command() = default;
  // Every specific command will implement this differently
  virtual void execute(const std::vector<std::string>& args) = 0;
};

class Exit : public Command {
public:
  void execute(const std::vector<std::string>& args) override {
    exit(0);
  }
};

class Echo : public Command {
public:
  void execute(const std::vector<std::string>& args) override {
    for (size_t i = 1; i < args.size(); i++) {
      std::cout << args[i];
      if (i < args.size() - 1) {
        std::cout << " ";
      }
    }
    std::cout << std::endl;
  }
};

class Type : public Command {
private:
  // List of builtin commands
  const std::array<std::string, 5> builtin {"echo", "exit", "type", "pwd", "cd"};
public:
  void execute(const std::vector<std::string>& args) override {
    bool is_builtin = false;
    std::string commandToFind {args[1]};
    for(size_t index = 0; index < builtin.size(); index++) {
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
};

class Pwd : public Command {
public:
  void execute(const std::vector<std::string>& args) override {

  }
};

class Cd : public Command {
public:
  void execute(const std::vector<std::string>& args) override {

  }
};

class Shell {
private:
  const std::string PATH = "PATH";
public:
  ParseState state = ParseState::Normal;
};

class Parser {
public:
  std::vector<std::string> parseLine(const std::string& line) {
    std::vector<std::string> tokens {};
    std::string currentToken {};
    ParseState state = ParseState::Normal;

    std::stringstream ss {line};
    char c {};    
    
    // Loop through all characters
    while(ss >> std::noskipws >> c) {
      if (c == '\'' && (state==ParseState::Normal || state==ParseState::NormalAfterSpace)) { // Opening quote
        state = ParseState::InSingleQuote;
      }
      else if (c == '\'' && state==ParseState::InSingleQuote) { // Ending quote          
        state = ParseState::Normal;       
      }
      else if (state==ParseState::InSingleQuote) { // Between single quotes
        currentToken += c;
      }
      else if (state==ParseState::Normal) {
        if (c == ' ') { // The end of a word
          state = ParseState::NormalAfterSpace;
          // Add the word to the list
          if (!currentToken.empty()) {
            tokens.push_back(currentToken);
            currentToken.clear();
          }       
        }
        else {            
          currentToken += c;
        }
      }
      else if (state==ParseState::NormalAfterSpace) {
        if (c != ' ') {
          state = ParseState::Normal;
          currentToken += c;
        }
      }
    }
    // Catch the last word if the line didn't end with a space
    if (!currentToken.empty()) {
      tokens.push_back(currentToken);
    }

    return tokens;
  }
};

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::string line {};
  std::string command {};
  
  // Instantiate parser
  Parser parser {};
  // Loop infinitely until user calls `exit` command.
  while(1) {
    std::cout << "$ ";
    std::getline(std::cin, line);

    std::stringstream ss {line};
    ss >> command;
    ss >> std::ws; // consume whitespace after command

    // Get the parsed tokens
    std::vector<std::string> tokens = parser.parseLine(line);
    if (tokens.empty()) continue;
    command = tokens[0];

    if (command == "exit") {
      Exit exitCmd;
      exitCmd.execute(tokens);
    }
    else if (command == "echo") {      
      Echo echoCmd;
      echoCmd.execute(tokens);
    }
    else if (command == "type") {
      Type typeCmd;
      typeCmd.execute(tokens);
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
          std::string args {};
          std::getline(ss, args);         
          std::string commandToExecute {command + " " + args};
          // Execute the command          
          std::system(commandToExecute.c_str());
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
