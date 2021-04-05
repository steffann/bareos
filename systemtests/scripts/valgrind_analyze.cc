#include <fstream>
#include <iostream>
#include <filesystem>
#include <regex>
#include <iostream>
#include <charconv>
#include "json/json.h"

namespace fs = std::filesystem;
std::regex filenameregex(R"(MemoryChecker\.([0-9]+)\.log)");

std::regex def_lost_regex(
    R"(.*definitely lost: ([0-9]+) bytes in ([0-9]*) blocks)");

std::regex def_lost_regex(
    R"(.*definitely lost: ([0-9]+) bytes in ([0-9]*) blocks)");

int main()
{
  Json::Value root = {};
  for (const auto& p : fs::recursive_directory_iterator(".")) {
    std::cmatch m;
    std::string current_path_string = p.path().filename().string();
    if (std::regex_search(current_path_string.c_str(), m, filenameregex)) {
      std::string testnumber = m[1].str();
      /* std::cout << "testnumber: >>>>" << testnumber << "<<<<" << std::endl;
       */
      /* std::cout << p.path().filename().c_str() << std::endl; */
      std::string line;
      auto file = std::ifstream((p.path()));
      while (std::getline(file, line)) {
        std::cmatch n;
        if (std::regex_search(line.c_str(), n, def_lost_regex)) {
          /* std::cout << line << std::endl; */
          /* std::cout << n[1] << " " << n[2] << std::endl; */
          root[testnumber]["definitely_lost"] = n[1].str();
          root[testnumber]["possibly_lost"] = n[2].str();
        }
      }
    }
  }
  std::cout << root;
}
