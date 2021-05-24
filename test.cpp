#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

void tokenize(std::string line, std::string target, const std::string del = ":") {
  std::string user, uid;
  int start = 0;
  int end = line.find(del);
  for (int pos = 0; pos < 3; pos++) {
    if (pos == 0)
      user = line.substr(start, end - start);
    else if (pos == 2 && target == line.substr(start, end - start)) {
      uid = line.substr(start, end - start);
      std::cout << "(FOUND) ";
    }

    start = end + del.size();
    end = line.find(del, start);
  }

  std::cout << uid << ": " << user << std::endl;
}

int main() {
    std::string target = "1000";
    std::ifstream stream("/etc/passwd");
    if (stream.is_open()) {
      std::string line, user, id;
      while (getline(stream, line)) {
        tokenize(line, target);  // , ":");

        // std::cout << id << ": " << user << std::endl;
      }
    }

//   std::string uid;
//   std::ifstream stream("/proc/990797/status");
//   if (stream.is_open()) {
//     std::string line;
//     while (std::getline(stream, line)) {
//       std::string str;
//       std::istringstream linestream(line);
//       linestream >> str;
//       std::cout << "Searching " << str << std::endl;
//       if (str == "Uid:") {
//         std::cout << "uid found!!" << std::endl;
//         linestream >> uid;
//         std::cout << uid << std::endl;
//         break;
//       }
//     }
//   }
}