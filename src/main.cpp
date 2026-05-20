#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::string userInput {};

  std::cout << "$ ";
  std::cin >> userInput;
  std::cout << userInput << ": command not found\n";
}
