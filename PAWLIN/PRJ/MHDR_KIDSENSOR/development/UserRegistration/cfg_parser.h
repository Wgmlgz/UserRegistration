#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>

class CfgParser {
 public:
  std::map<std::string, std::vector<std::string>> values;
  CfgParser(std::string path = "config.txt");
  std::string Get(std::string key);
  std::vector<std::string> GetArray(std::string key);
};

CfgParser::CfgParser(std::string path) {
  std::ifstream cFile(path);
  if (cFile.is_open()) {
    std::string line;
    while (getline(cFile, line)) {
      line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
      if (line[0] == '#' || line.empty()) continue;
      auto delimiterPos = line.find("=");
      auto name = line.substr(0, delimiterPos);
      auto value = line.substr(delimiterPos + 1);

      vector<string> result;
      std::stringstream ss(value);
      while (ss.good()) {
        string substr;
        std::getline(ss, substr, ',');
        result.push_back(substr);
      }

      values.insert(std::make_pair(name, result));
      std::cout << name << " " << value << '\n';
    }
  } else {
    std::cerr << "Couldn't open config file for reading.\n";
  }
}

std::string CfgParser::Get(std::string key){
  return values.find(std::string(key))->second[0];
}
std::vector<std::string> CfgParser::GetArray(std::string key) {
  return values.find(std::string(key))->second;
}