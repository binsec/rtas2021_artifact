#ifndef __DEBUG_LOG_HH__
#define __DEBUG_LOG_HH__

using namespace std;

#include <iostream>
#include <fstream>
#include <map>

class LogClass
{private:
  map<string,ofstream*> streams;
 public:
  
  ofstream & operator()(const string &s)
  { map<string,ofstream*>::iterator it = streams.find(s);
    if(it==streams.end())
    { // No such a logfile already exists
      streams[s] = new ofstream(s.c_str(),ios_base::trunc & ios_base::out);
      return *(streams[s]);
    }
    else return *(it->second);
  }
  

};

LogClass LOG;

#endif
