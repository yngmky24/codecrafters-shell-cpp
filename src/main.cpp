#include <iostream>
#include <string>
#include <cstdlib>

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
    if (userInput.substr(0, 5) == "echo ") {
      std::cout << userInput.substr(5) << '\n';
    }
    else {
      std::cout << userInput << ": command not found\n";
    }
  }
}
