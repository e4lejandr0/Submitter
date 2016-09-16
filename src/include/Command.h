#ifndef SICT_COMMAND_H_
#define SICT_COMMAND_H_

#include <string>
namespace sict{

  class Command :public std::string{ //a command is a string?? | composition over inheritance though
  public:
    Command(std::string str);
    int run()const;
  };
}
#endif
