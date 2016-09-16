#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "include/Vals.h"
#include "include/SubVals.h"
#include "include/Command.h"
#include "include/Submitter.h"
using namespace std;
namespace sict{
  Submitter::Submitter(int argc, char** argv) :_cls("clear"){
    _argc = argc;
    _argv = argv;
    _home = argv[0];
    size_t last = _home.find_last_of('/');
    if (last != string::npos){
      _home = _home.substr(0, last + 1);
    }
    ok2submit = true;
  }
  void Submitter::clrscr()const{
    _cls.run();
  }
  void Submitter::setSubmitterDir(){
    bool done = false;
    Vals V('|');
    string  path = _home + SUB_CFG_FILE;
    ifstream file(path.c_str());
    while (file && !done){
      file >> V;  // read a line of config file
      if (V.size() > 0){ // if any values read
        done = true;
        if (V[0] == "dir"){
          _submitterDir = _home + V[1];
        }
      }
    }
    file.close();
    if (_submitterDir.length() == 0) _submitterDir = _home + SUB_DEF_DIR;
  }
  bool Submitter::getAssignmentValues(){
    bool ok = false;
    Vals V('|');
    std::string fname(_submitterDir + "/" + _argv[1]);
    fname += ".cfg";
    ifstream file(fname.c_str());
    while (file){
      file >> V;
      if (V.size() > 1){
        ok = true;
        _AsVals.add(V[0], Vals(V[1], ','));
      }
    }
    file.close();
    return ok;
  }
  bool Submitter::copyProfFiles(){
    bool ret = true;
    if (_AsVals.exist("copy_files")){
      ret = _AsVals["copy_files"].size() > 0;
      int i;
      for (i = 0; ret && i < _AsVals["copy_files"].size(); i++){
        Command cmd("cp ");
        cmd += (_submitterDir + "/" + _AsVals["copy_files"][i] + " .");
        // cout << cmd << endl; // to show or not to show!
        ret = (cmd.run() == 0);
      }
    }
    return ret;
  }
  bool Submitter::filesExist(){
    Vals& files = _AsVals["assess_files"];
    bool ret = files.size() > 0;
    ifstream file;
    int i;
    for (i = 0; ret && i < files.size(); i++){
      file.open(files[i]);
      if (!file){
        ok2submit = ret = false;
        cout << files[i] << ", is missing!" << endl;
      }
      else{
        file.close();
      }
      file.clear();
    }
    return ret;
  }
  class Line{
    const char* _line;
  public:
    Line(const char* line){
      _line = line;
    }
    ostream& display(ostream& os)const{
      const char* line = _line;
      while (*line){
        if (*line == '\b'){
          os << "\\b";
        }
        else{
          os << *line;
        }
        line++;
      }
      return os;
    }
  };
  ostream& operator<<(ostream& os, const Line& L){
    return L.display(os);
  }
  void Submitter::diff(ostream& os, const char* stdnt, const char* prof, int line){
    int i = 0;
    os << endl<< "In line number " << line << " of your output:" << endl;
    os << "The output should be:" << endl << Line(prof) << endl;
    os << "But your output is: " << endl << Line(stdnt) << endl;
    while (stdnt[i] && prof[i] && stdnt[i] == prof[i]){
      os << " ";
      i++;
    }
    os << "^" << endl;
    i = 0;
    while (stdnt[i] && prof[i] && stdnt[i] == prof[i]){
      os << "-";
      i++;
    }
    os << "|" << endl;
    os << "Professors character ASCII code: hex(" << hex << int(prof[i])
      << "), dec(" << dec << int(prof[i]) << ")" << endl;
    os << "un-matched character ASCII code: hex(" << hex << int(stdnt[i])
      << "), dec(" << dec << int(stdnt[i]) << ")" << endl;

  }
  bool Submitter::compare(const char* stdnt, const char* prof, int line){
    bool ok = strcmp(stdnt, prof) == 0;
    if (!ok){
      diff(cout, stdnt, prof, line);
      ofstream diffFile("diff.txt");
      diff(diffFile, stdnt, prof, line);
      diffFile.close();
    }
    return ok;
  }
  bool Submitter::removeBS(){
    bool good = true;
    fstream file(_AsVals["output_file"][0].c_str(),ios::in);
    if (!file){
      cout << "Error #17.1: cound not open " << _AsVals["output_file"][0] << endl;
      good = false;
    }
    else{
      int size;
      file.seekg((ios::off_type)0, ios::end);
      size = file.tellg();
      char* buf = new char[size + 1];
      file.seekg((ios::off_type)0);
      char ch;
      int i = 0;
      while (!file.fail()){
        ch = file.get();
        if (!file.fail()){
          if (ch == '\b'){
            if (i>0) i--;
          }
          else{
            buf[i++] = ch;
          }
        }
      }
      buf[i] = 0;
      file.close();
      file.open(_AsVals["output_file"][0].c_str(), ios::out);
      if (!file){
        cout << "Error #17.2: cound not open " << _AsVals["output_file"][0] <<" for output"<< endl;
        good = false;
      }
      else{
        i = 0;
        while (buf[i]){
          file.put(buf[i++]);
        }
        file.close();
      }
      delete[] buf;
    }
    return good;
  }
  bool Submitter::compareOutputs(int from, int to){
    char sstr[256], pstr[256];
    bool good = true;
    int line = 0;
    ifstream stfile(_AsVals["output_file"][0].c_str());
    ifstream prfile(_AsVals["correct_output"][0].c_str());
    if (!stfile){
      cout << "Error #17: cound not open " << _AsVals["output_file"][0] << endl;
      good = false;
    }
    if (!prfile){
      cout << "Error #17: cound not open " << _AsVals["correct_output"][0] << endl;
      good = false;
    }
    while (line < to && good && stfile && prfile){
      line++;
      sstr[0] = pstr[0] = 0;
      stfile.getline(sstr, 255, '\n');
      prfile.getline(pstr, 255, '\n');
      if (line >= from){
        ok2submit = good = compare(sstr, pstr, line);
      }
    }
    if (line < from){
      ok2submit = good = false;
      cout << "Your output file is too short or empty!" << endl;
    }
    return good;
  }
  int Submitter::compile(){
    int bad = 0;
    int i = 0;
    int errcode = 0;
    if (_AsVals.exist("compile_command")){
      Command compile(_AsVals["compile_command"][1]);
      if (_AsVals.exist("compile_files")){
        for (i = 0; i < _AsVals["compile_files"].size(); i++){
          compile += (" " + _AsVals["compile_files"][i]);
        }
        if (_AsVals.exist("err_file")){
          compile += (" 2> " + _AsVals["err_file"][0]);
          cout << compile << endl;
          if ((errcode = compile.run()) != 0){
            cout << "You have compilation errors. Please open \"" << _AsVals["err_file"][0] << "\" to veiw" << endl
              << "and correct them." << endl << "Submission aborted! (code: " <<errcode <<")"<< endl;
            ok2submit = false;
            bad = 9;
          }
          if (!bad && _AsVals["allow_warning"][0] != "yes"){
            if (Command("grep warning " + _AsVals["err_file"][0] + ">/dev/null").run() == 0){
              cout << "You have compilation warnings. Please open \"" << _AsVals["err_file"][0] << "\" to veiw" << endl
                << "and correct them." << endl << "Submission aborted!" << endl;
              bad = 10;
              ok2submit = false;
            }
          }
        }
        else{
          cout << "Error #8: error log filename not specified!" << endl
            << "Please report this to your professor." << endl;
          bad = 8;
        }
      }
      else{
        cout << "Error #7: file names to be compiled not specified!" << endl
          << "Please report this to your professor." << endl;
        bad = 7;
      }
    }
    else {
      cout << "Error #6: Compile command not found!" << endl
        << "Please report this to your professor." << endl;
      bad = 6;
    }
    return bad;
  }
  int Submitter::execute(){
    int bad = 0;
    if (!_AsVals.exist("exe_name")){
      cout << "Error #11: executable filename not specified!" << endl
        << "Please report this to your professor." << endl;
      bad = 11;
    }
    else if (!_AsVals.exist("output_file")){
      cout << "Error #12: output filename not specified!" << endl
        << "Please report this to your professor." << endl;
      bad = 12;
    }
    else{
      if (_AsVals["output_type"][0] == "script"){
        cout << "\n\nREAD THE FOLLOWING CAREFULLY!" << endl;
        cout << "I am about to execute the tester and script the output to \"" << _AsVals["output_file"][0] << "\"" << endl;
        cout << "Please enter the values carefuly and exactly as instructed." << endl
          << "Press <ENTER> to start...";
        cin.ignore(1000, '\n');
        clrscr();
        cout << "FOLLOW THESE INSTRUCTIONS:" << endl;
        cout << "1- Type " << _AsVals["exe_name"] << " and hit <ENTER>" << endl;
        cout << "2- Use the test data sepcified in " << name() << " to test the program" << endl;
        cout << "3- When done, type exit and hit <ENTER> to continue the sumbission process." << endl;
        Command("script " + _AsVals["output_file"][0]).run();
      }
      else{
        Command(_AsVals["exe_name"] + " >" + _AsVals["output_file"]).run();
      }
    }
    return bad;
  }
  int Submitter::checkOutput(){
    int bad = 0;
    int from = 0, to = 0;
    if (!removeBS()) bad = 17;
    if (!bad && _AsVals.exist("comp_range")){
      if (sscanf(_AsVals["comp_range"][0].c_str(), "%d", &from) == 1
        && sscanf(_AsVals["comp_range"][1].c_str(), "%d", &to) == 1){
        if (_AsVals.exist("correct_output")){
          if (Command("cp " + _submitterDir + "/" + _AsVals["correct_output"][0] + " .").run() == 0){
            if (!compareOutputs(from, to)){
              bad = 18;
              cout << "Outputs don't match. Submission aborted!" << endl;
            }
            else{
              cout << "Success!... Outputs match." << endl;
            }
          }
          else{
            cout << "Error #15: could not access " << _AsVals["correct_output"][0] << "." << endl
              << "please report this to you professor!" << endl;
            bad = 15;
          }
        }
        else{
          cout << "Error #14: \"correct output\" is not specified!" << endl
            << "please report this to you professor!" << endl;
          bad = 14;
        }
      }
      else{
        cout << "Error #16: bad \"comparison range\" values!" << endl
          << "please report this to you professor!" << endl;
      }
    }
    else{
      cout << "Error #13: \"comparison range\" is not specified!" << endl
        << "please report this to you professor!" << endl;
      bad = 13;
    }
    return bad;
  }
  const char* Submitter::name(){
    return _AsVals["assessment_name"][0].c_str();
  }

  /*run returns:
  0 OK
  1 incorrect command line argument
  2 can not open submitter specs file
  3 assessment name missing in assignment configuratrion file
  4 Student does not have all the files available in submission directory
  5 tester files missing in professor's direcotry or directory not accessible
  6 Complier command not specified in config file
  7 file names to be compiled are not specified in the assessment config file!
  8 the err_file value is not sepcified in the assessment config file
  9 compile error in student assignment
  10 compile warning in student assignment
  11 the exe_name value is not specified in the assessment config file
  12 the output_file value is not specified in the assessment config file
  13 the comp_range value is not specified in the assessment config file
  14 the correct_output value is not specified in the assessment config file
  15 failed to copy the correct output from submitter dir. (bad permission setting?)
  16 the comp_range, line number values in the config file could not be read as integers
  17 could not open the output files
  18 output comparison failed
  19 the linux mail command failed at the end
  */
  int Submitter::run(){
    int bad = 0;
    int i = 0;
    clrscr();
    cout << "Submitter (V" << SUBMITTER_VERSION << ")" << endl;
    // if the command has valid format
    if (_argc != 2){
      cout << "Error #1: submsstion command format: " << endl;
      cout << "~prof_name.prof_lastname/submit AssignmentName<ENTER>" << endl;
      bad = 1;
    }
    if (!bad){
      // from SUB_CFG_FILE file set the sibmitter directory
      setSubmitterDir();
      // get the assignment specs and put it in AsVals
      bad = int(!getAssignmentValues()) * 2;
      bad && cout << "Error #2: Can not find the assignment submission sepcs!" << endl
        << "Please report this to your professor!" << endl;
    }
    if (!bad){
      // if Assignment name is set in the assignment spcs files
      if (_AsVals.exist("assessment_name")){
        cout << "Submitting " << name() << endl;
      }
      else{ // otherwise exit with error
        cout << "Error #3: \"assessment_name\" is not specified!" << endl
          << "please report this to you professor!" << endl;
        bad = 3;
      }
    }
    if (!bad){
      // check to make sure all files to be submitted is in
      // the currect dir.
      // fileExist function already prints the name of missing files
      bad = int(!filesExist()) * 4;
      bad && cout << "Error #4: submission files missing." << endl;
      ok2submit = !bad;
    }
    if (!bad){
      bad = int(!copyProfFiles()) * 5;
      bad && cout << "Error #5: Could not copy tester files!" << endl << "Please report this to your professor!" << endl;
    }
    if (!bad && _AsVals["compile"][0] == "yes"){
      if ((bad = compile()) == 0){
        cout << "Success! no errors or warnings..." << endl;
      }
    }
    if (!bad && _AsVals["exectue"][0] == "yes"){
      bad = execute();
    }
    if (!bad && _AsVals["check_output"][0] == "yes"){
      bad = checkOutput();
    }
    if (!bad && ok2submit){
      cout << endl << "Would you like to submit this demonstration of " << name() << "? (Y)es/(N)o: ";
      char res = cin.get();
      cin.ignore(1000, '\n');
      if (res == 'Y' || res == 'y'){
        if (submit(_AsVals["prof_email"][0])){
          cout << "Thank you!, Your work is now submitted." << endl;
        }
        else{
          bad = 19;
          cout << "Error #19: email failed." << endl
            << "Please report this to your professor" << endl;
        }
        if (!bad && _AsVals["prof_email"].size() > 1){
          cout << endl << "Would you like to submit a copy of this demonstration of " << name() << " to the TA for feedback? (Y)es/(N)o: ";
          char res = cin.get();
          cin.ignore(1000, '\n');
          if (res == 'Y' || res == 'y'){
            for (i = 1; i < signed(_AsVals["prof_email"].size()); i++){
              if (submit(_AsVals["prof_email"][i])){
                cout << "CC no " << i << " is sent to the TA for feedback." << endl;
              }
              else{
                bad = 19;
                cout << "Error #19: email CC failed." << endl
                  << "Please report this to your professor" << endl;
              }
            }
          }
        }
      }
      else{
        cout << "Submission aborted by user!" << endl;
      }
    }
    return bad;
  }
  bool Submitter::submit(string& toEmail){
    Command email("echo \"");
    email += name();
    email += " submission by `whoami`. Executed from ";
    email += _home;
    email += "\" | mail -s \"";
    email += _AsVals["subject_code"][0] + " - ";
    email += name();
    email += " submission by `whoami`\" ";
    for (int i = 0; i < _AsVals["submit_files"].size(); i++){
      email += " -a " + _AsVals["submit_files"][i];
    }
    email += " " + toEmail;
    email += " -- -f `whoami`@myseneca.ca";
    // cout << email << endl;  // fardad was here
    return email.run() == 0;
  }
}






