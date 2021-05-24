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
  Process(int pid) : pid_(pid) { ComputeCpuUtilization(); }

  // Added const keyword to protect the data
  int Pid() const;                         // TODO: See src/process.cpp
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization() const;            // TODO: See src/process.cpp
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process const & a) const;  // TODO: See src/process.cpp

  // TODO: Declare any necessary private members
 // Won't change throughout the lifetime of the process
 private:
  int pid_;
  float cpu_usage_;
  std::string user_{std::string()};
  std::string command_{std::string()};

  // Helper method to compute cpu utilization
  void ComputeCpuUtilization();

  // Save the previous timing of the processes and share them across 
  // process instances, so that we can compute more recent CPU usage of the 
  // processes. key: pid; value: (total_time, seconds)
  // NOTE: need to remove the obsolete processes
  // Stuck here. Not sure how to calculate the current CPU load.
  // Suggestions?
  // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  static std::map<int, std::tuple<float, float>> process_loads;

};

#endif