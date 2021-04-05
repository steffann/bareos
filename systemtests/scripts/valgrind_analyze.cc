#include <fstream>
#include <iostream>
#include <filesystem>
#include <regex>
#include <iostream>
#include <charconv>
#include "json/json.h"

namespace fs = std::filesystem;
std::regex filenameregex(R"(MemoryChecker\.([0-9]+)\.log)");

std::regex definitely_lost_regex(
    R"(.*definitely lost: ([0-9]+) bytes in ([0-9]*) blocks)");

std::regex possibly_lost_regex(
    R"(.*possibly lost: ([0-9]+) bytes in ([0-9]*) blocks)");

int main()
{
  int total_def_lost_bytes{};
  int total_pos_lost_bytes{};
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
        if (std::regex_search(line.c_str(), n, definitely_lost_regex)) {
          root[testnumber]["definitely_lost_bytes"] = n[1].str();
          total_def_lost_bytes += std::stoi(n[1].str());
          root[testnumber]["definitely_lost_blocks"] = n[2].str();
        }
        if (std::regex_search(line.c_str(), n, possibly_lost_regex)) {
          root[testnumber]["possibly_lost_bytes"] = n[1].str();
          total_pos_lost_bytes += std::stoi(n[1].str());
          root[testnumber]["possibly_lost_blocks"] = n[2].str();
        }
      }
    }
    root["TOTALS"]["definitely_lost_bytes"] = total_def_lost_bytes;
    root["TOTALS"]["possibly_lost_bytes"] = total_pos_lost_bytes;
  }
  std::cout << root;
}
