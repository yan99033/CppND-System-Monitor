#include "process.h"
#include "linux_parser.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include <iostream>

using std::string;
using std::to_string;
using std::vector;

std::map<int, std::tuple<float, float>> Process::process_loads;

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { 
  float total_time = (float)LinuxParser::ActiveJiffies(pid_);
  float elapsed = (float)LinuxParser::UpTime(pid_);
  float cpu_usage = (total_time / sysconf(_SC_CLK_TCK)) / elapsed;

  // // Previous timing found
  // if (Process::process_loads.find(pid_) != Process::process_loads.end()) {
  //   float total_time_prev = std::get<0>(Process::process_loads[pid_]);
  //   float elapsed_prev = std::get<1>(Process::process_loads[pid_]);
  //   cpu_usage = (total_time - total_time_prev) / (elapsed - elapsed_prev);
  // }
  // else {
  //   cpu_usage = (total_time / sysconf(_SC_CLK_TCK)) / elapsed;
  // }
  // // Save the timing for future use
  // Process::process_loads[pid_] = std::make_tuple(total_time, elapsed);

  return cpu_usage;
}

// TODO: Return the command that generated this process
string Process::Command() { 
  if (command_.empty())
    command_ = LinuxParser::Command(pid_);
  return command_;
}

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const 
{ 
  return a.CpuUtilization() < CpuUtilization(); 
}