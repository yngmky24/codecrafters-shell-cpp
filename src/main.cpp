#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <filesystem>
#include <unordered_map>
#include <memory>
namespace fs = std::filesystem;

enum class ParseState {
  Normal,
  NormalAfterSpace,
  InSingleQuote,
  InDoubleQuote
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
    // Print working directory
    std::cout << fs::current_path().string() << std::endl;
  }
};

class Cd : public Command {
public:
  void execute(const std::vector<std::string>& args) override {
    std::string destPath {args[1]};
    std::string errorMsg {"cd: " + destPath + ": No such file or directory"};

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
        std::cerr << errorMsg << std::endl;
      }
    }
  }
};

class Undefined : public Command {
public:
  void execute(const std::vector<std::string>& args) override {
    std::string command {args[0]};
    std::string commandArgs {""};
    for (size_t i = 1; i < args.size(); i++) {
      commandArgs += "'" + args[i] + "' ";      
    }

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
        std::string commandToExecute {command + " " + commandArgs};
        // Execute the command          
        std::system(commandToExecute.c_str());
        is_executable = true;
        break;
      }
    }

    // If the given command is non-executable return default message.
    if (!is_executable) {
      std::cerr << command << ": command not found" << std::endl;
    }
  }
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
      if (c == '\'' && (state==ParseState::Normal || state==ParseState::NormalAfterSpace)) {
        // Begin single quote
        state = ParseState::InSingleQuote;
      }
      else if (c == '\'' && state==ParseState::InSingleQuote) { 
        // End single quote          
        state = ParseState::Normal;       
      }
      else if (state==ParseState::InSingleQuote) { 
        // Between single quotes
        currentToken += c;
      }
      else if (c == '\"' && (state==ParseState::Normal || state==ParseState::NormalAfterSpace)) {
        // Begin double quote
        state = ParseState::InDoubleQuote;
      }
      else if (c == '\"' && state==ParseState::InDoubleQuote) {
        // End of double quote
        state = ParseState::Normal;
      }
      else if (state==ParseState::InDoubleQuote) {
        currentToken+=c;
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

class Shell {
private:
  const std::string PATH = "PATH";
  const std::string HOME = "HOME";
  Parser parser;
  std::unordered_map<std::string, std::unique_ptr<Command>> builtins {};

public:
  Shell() {
    builtins["exit"] = std::make_unique<Exit>();
    builtins["echo"] = std::make_unique<Echo>();
    builtins["type"] = std::make_unique<Type>();
    builtins["pwd"]  = std::make_unique<Pwd>();
    builtins["cd"]   = std::make_unique<Cd>();
  }

  void run() {
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
    
      if(builtins.find(command) != builtins.end()) {
        builtins[command]->execute(tokens);
      }
      else {
        Undefined undefinedCmd;
        undefinedCmd.execute(tokens);
      }
    }
  }
};

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  Shell myShell;
  myShell.run();

  return 0;
}
