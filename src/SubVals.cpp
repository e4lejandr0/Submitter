#include "include/Vals.h"
#include "include/SubVals.h"
using namespace std;
namespace sict{
  SubVal::SubVal(std::string& key, Vals& value){
    _key = key;
    _value = value;
  }
  void SubVals::add(string key, Vals value){
    _vals.push_back(SubVal(key, value));
  }
  string& SubVals::operator[](int index){
    return _vals[index % _vals.size()]._key;
  }
  bool SubVals::exist(const char* key)const{
    bool ret = false;
    unsigned i = 0;
    for (i = 0; ret == false && i < _vals.size(); i++){
      ret = (_vals[i]._key == key);
    }
    return ret;
  }
  Vals& SubVals::operator[](const char* key){
    Vals* sv = &_nothing; //???
    unsigned i = 0;
    for (i = 0; i < _vals.size() && sv->size() == 0; i++){
      if (_vals[i]._key == key){
        sv = &_vals[i]._value;
      }
    }
    return *sv;
  }
  int SubVals::size(){
    return _vals.size();
  }
}
