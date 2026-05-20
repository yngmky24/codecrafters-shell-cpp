#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::string userInput {};
  while(1) {
    std::cout << "$ ";
    std::cin >> userInput;
    if (userInput == "exit") {
      exit;
    }
    std::cout << userInput << ": command not found\n";
  }
}
