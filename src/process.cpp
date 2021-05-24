#include "process.h"
#include "linux_parser.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include <math.h>

#include <iostream>

using std::string;
using std::to_string;
using std::vector;

std::map<int, std::tuple<float, float>> Process::process_loads;

// TODO: Return this process's ID
int Process::Pid() const { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { 
  return cpu_usage_;
}
  
void Process::ComputeCpuUtilization() {
  float total_time = (float)LinuxParser::ActiveJiffies(pid_);
  float elapsed = (float)LinuxParser::UpTime(pid_);
  // float cpu_usage = (total_time / sysconf(_SC_CLK_TCK)) / elapsed;

  float total_time_prev = 0, elapsed_prev = 0;
  if (Process::process_loads.find(pid_) != Process::process_loads.end()) {
    total_time_prev = std::get<0>(Process::process_loads[pid_]);
    elapsed_prev = std::get<1>(Process::process_loads[pid_]);
  }
  
  float totald, idled;
  // Need to make sure the timestamps don't coincide
  // So we can sample the CPU load at two distinct timestamps
  if (elapsed - elapsed_prev > 0) {
    float idle = elapsed - total_time;
    float idle_prev = elapsed_prev - total_time_prev;

    totald = elapsed - elapsed_prev;
    idled = idle - idle_prev;

    // Save the timing for future use
    Process::process_loads[pid_] = std::make_tuple(total_time, elapsed);

    cpu_usage_ = ((totald - idled) / sysconf(_SC_CLK_TCK)) / totald;

    // Capped at 100 %
    if (cpu_usage_ > 1)
      cpu_usage_ = 1;
  }
  else {
    cpu_usage_ = (total_time / sysconf(_SC_CLK_TCK)) / elapsed;
  }
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