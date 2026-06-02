#include "../include/Builtins.hpp"
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
namespace fs = std::filesystem;

void Exit::execute(const std::vector<std::string>& args) {
  exit(0);
}

void Echo::execute(const std::vector<std::string>& args) {
  for (size_t i = 1; i < args.size(); i++) {
    std::cout << args[i];
    if (i < args.size() - 1) {
      std::cout << " ";
    }
  }
  std::cout << std::endl;
}

void Type::execute(const std::vector<std::string>& args) {
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

void Pwd::execute(const std::vector<std::string>& args) {          
  // Print working directory
  std::cout << fs::current_path().string() << std::endl;
}

void Cd::execute(const std::vector<std::string>& args) {
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

void Undefined::execute(const std::vector<std::string>& args) {
  std::string command {args[0]};
  bool is_executable {false};
  
  // Determine if the given command is an executable in the PATH
  std::string path_env = std::getenv("PATH");
  std::stringstream ss_path(path_env);
  std::string path;

  // std::string commandArgs {""};
  // for (size_t i = 1; i < args.size(); i++) {
  //   commandArgs += "'" + args[i] + "' ";      
  // }

  while(std::getline(ss_path, path, ':')) {
    fs::path p {path};
    fs::path fullPath {p / command};

    auto exec_mask = fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec;
    fs::perms permission {fs::status(fullPath).permissions()};

    if (fs::exists(fullPath) && (permission & exec_mask) != fs::perms::none) {
      // Pass any arguments from the command line     
      // std::string commandToExecute {command + " " + commandArgs};
      // Execute the command          
      // std::system(commandToExecute.c_str());
      is_executable = true;
      // 1. Prepare arguments for execv
      // execv requires an array of C-style strings (char*), ending with a nullptr
      std::vector<char*> c_args;
      for (const auto&arg : args) {
        c_args.push_back(const_cast<char*>(arg.c_str()));
      }
      c_args.push_back(nullptr);

      // 2. Fork the process
      pid_t pid = fork();

      if(pid == 0) {
        // In the child process
        // Replace the child process with the target executable
        execv(fullPath.c_str(), c_args.data());

        // If execv returns it means it failed
        std::cerr << "Execution failed\n";
        exit(1);
      }
      else if (pid > 0) {
        // In the parent process
        // Wait for the child program to finish before printing the next `$`
        int status;
        waitpid(pid, &status, 0); 
      }
      else {
        std::cerr << "Fork failed\n";
      }

      break;
    }
  }

  // If the given command is non-executable return default message.
  if (!is_executable) {
    std::cerr << command << ": command not found" << std::endl;
  }
}
