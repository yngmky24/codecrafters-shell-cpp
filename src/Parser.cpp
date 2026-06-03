#include "../include/Parser.hpp"
#include <ios>
#include <sstream>

std::vector<std::string> Parser::parseLine(const std::string& line) {
  std::vector<std::string> tokens {};
  std::string currentToken {};
  ParseState state = ParseState::Normal;

  std::stringstream ss {line};
  char c {};    
  
  // Loop through all characters
  // while(ss >> std::noskipws >> c) {
  //   if (c == '\'' && (state==ParseState::Normal || state==ParseState::NormalAfterSpace)) {
  //     // Begin single quote
  //     state = ParseState::InSingleQuote;
  //   }
  //   else if (c == '\'' && state==ParseState::InSingleQuote) { 
  //     // End single quote          
  //     state = ParseState::Normal;       
  //   }
  //   else if (state==ParseState::InSingleQuote) { 
  //     // Between single quotes
  //     currentToken += c;
  //   }
  //   else if (c == '\"' && (state==ParseState::Normal || state==ParseState::NormalAfterSpace)) {
  //     // Begin double quote
  //     state = ParseState::InDoubleQuote;
  //   }
  //   else if (c == '\"' && state==ParseState::InDoubleQuote) {
  //     // End of double quote
  //     state = ParseState::Normal;
  //   }
  //   else if (state==ParseState::InDoubleQuote) {
  //     currentToken += c;
  //   }
  //   else if (state==ParseState::Normal) {
  //     if (c == ' ') { // The end of a word
  //       state = ParseState::NormalAfterSpace;
  //       // Add the word to the list
  //       if (!currentToken.empty()) {
  //         tokens.push_back(currentToken);
  //         currentToken.clear();
  //       }       
  //     }
  //     else {            
  //       currentToken += c;
  //     }
  //   }
  //   else if (state==ParseState::NormalAfterSpace) {
  //     if (c != ' ') {
  //       state = ParseState::Normal;
  //       currentToken += c;
  //     }
  //   }
  // }

  while (ss >> std::noskipws >> c) {
    switch (state) {
      case ParseState::Normal:
        if (c == '\'') {
          state = ParseState::InSingleQuote;
        }
        else if (c == '\"') {
          state = ParseState::InDoubleQuote;
        }
        else if (c == ' ') {
          if(!currentToken.empty()) {
            tokens.push_back(currentToken);
            currentToken.clear();
          }
        }
        else if (c == '\\') {
          state = ParseState::EscapedNormal;
        }
        else {
          currentToken += c;
        }
        break;
      case ParseState::InSingleQuote:
        if (c == '\'') {
          state = ParseState::Normal;          
        }
        else {
          currentToken += c;
        }
        break;
      case ParseState::InDoubleQuote: 
        if (c == '\"') {
          state = ParseState::Normal;
        }
        else if (c == '\\') {
          state = ParseState::EscapedDouble;
        }
        else {
          currentToken += c;
        }
        break;
      case ParseState::EscapedNormal:
        currentToken += c;
        state = ParseState::Normal;
        break;
      case ParseState::EscapedDouble:
        // Only escapes ", \, $, `, and newline
        const std::vector<char> specialChars {'"', '\\'};
        for (const auto spChar : specialChars) {
          if (c == spChar) {
            currentToken +=c;
          }
        }
        break;
    }
  }

  // Catch the last word if the line didn't end with a space
  if (!currentToken.empty()) {
    tokens.push_back(currentToken);
  }

  return tokens;
}
