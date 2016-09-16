#ifndef ICT_SUBVALS_H__
#define ICT_SUBVALS_H__
#include <string>
#include <vector>
#include "Vals.h"
namespace sict{
  class SubVals;
  class SubVal{
    std::string _key;
    Vals _value;
    SubVal(std::string& key, Vals& value);
    friend class SubVals;
  };
  class SubVals{
    Vals _nothing; //???
    std::vector<SubVal> _vals;
  public:
    void add(std::string key, Vals value);
    bool exist(const char* key)const;
    Vals& operator[](const char* key);
    std::string& operator[](int index);
    int size();
  };
}
#endif
