#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {
  
  ifstream in_file(argv[5]);
  string line;

  while (getline(in_file, line)) {

    std::string u = string("beep -f ").append(argv[1])
      .append(" -l ").append(argv[2]);
    
    std::string i = string("beep -f ").append(argv[3])
      .append(" -l ").append(argv[4]);
    int r = 0;
    //
    //r++;
    if (line != "U") {
      r = system(i.c_str());
      std::cout << line << std::endl;
      usleep(500000);
    } else if (line == "U") {
      r = system(u.c_str());
      usleep(5000);
    }
    r++;
  }
  
}
