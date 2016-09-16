#ifndef SICT_SUBMITTER_H_
#define SICT_SUBMITTER_H_
#include "Vals.h"
#include "SubVals.h"
#include "Command.h"
// define statements

//  version
#ifdef SUBMITTER_VERSION
# undef SUBMITTER_VERSION
#endif
#define SUBMITTER_VERSION "0.93 (Backspace allowed in script)"
//    program config file
#ifdef SUB_CFG_FILE
# undef SUB_CFG_FILE
#endif
#define SUB_CFG_FILE "submitter.cfg"


//    program default directory for assignment configuration files
#ifdef SUB_DEF_DIR
# undef SUB_DEF_DIR
#endif
#define SUB_DEF_DIR "submitter_files"

namespace sict{
  class Submitter{
    bool ok2submit;
    int _argc;
    char** _argv;
    std::string _home;
    std::string _submitterDir;  
    SubVals _AsVals;
    Command _cls;
    void setSubmitterDir();
    void clrscr()const;
    bool getAssignmentValues();
    bool copyProfFiles();
    bool filesExist();
    static bool compare(const char* stdnt, const char* prof, int line);
    static void diff(std::ostream& os, const char* stdnt, const char* prof, int line);
    int compile();
    int execute();
    int checkOutput();
    bool compareOutputs(int from, int to); 
    const char* name();
    bool submit(std::string& toEmail);
    bool removeBS();
  public:
    Submitter(int argc, char** argv);
    int run();
  };
}
#endif