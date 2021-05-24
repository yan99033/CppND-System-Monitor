#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <tuple>
#include <map>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid) : pid_(pid) {}
  Process(int pid, std::string user, std::string command, 
          long total_time, long elapsed, std::string ram, long int uptime) :
          pid_(pid), user_(user), command_(command), total_time_(total_time),
          elapsed_(elapsed), ram_(ram), uptime_(uptime) {}

  int Pid();                               // TODO: See src/process.cpp
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization();                  // TODO: See src/process.cpp
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp

  // TODO: Declare any necessary private members
 private:
  int pid_;
  std::string user_;
  std::string command_;
  float cpu_usage_;
  long total_time_;
  long elapsed_;
  std::string ram_;
  long int uptime_;

  // Save the previous timing of the processes and share them across 
  // process instances, so that we can compute more recent CPU usage of the 
  // processes. key: pid; value: (total_time, seconds)
  // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  static std::map<int, std::tuple<long, long>> process_loads;

};

#endif