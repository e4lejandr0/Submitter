//#define SICT_DEBUG
# ifdef SICT_DEBUG
#   include <iostream>
# endif
#include <cstdlib>
#include "include/Command.h"

using namespace std;
namespace sict{
  Command::Command(string str) : string(str){
  }
  int Command::run()const{
# ifdef SICT_DEBUG
    cout <<"RUN: "<< c_str() << endl;
    return 0;
# else
    return system(c_str());
# endif
  }
}


