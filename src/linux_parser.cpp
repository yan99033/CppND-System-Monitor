#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// (DONE) TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  // string line, str;
  float memtotal, memfree, buffers, cached, shmem, sreclaimable;  // num;
  // https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop/41251290#41251290
  // MemUsage = (MemTotal - MemFree) - (Buffers + Cached + SReclaimable - Shmem)
  // MemUsagePerc = MemUsage / MemTotal
  memtotal = memfree = buffers = cached = shmem = sreclaimable = 0;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    string line;
    while (getline(stream, line)) {
      string str;
      float num;
      std::istringstream linestream(line);
      linestream >> str >> num;

      if (str == "MemTotal:")
        memtotal = num;
      else if (str == "MemFree:")
        memfree = num;
      else if (str == "Buffers:")
        buffers = num;
      else if (str == "Cached:")
        cached = num;
      else if (str == "Shmem:")
        shmem = num;
      else if (str == "SReclaimable:") {
        sreclaimable = num;
        break;  // We know that this is the last number to extract
      }
    }
  }

  // This gets pretty close to the percentage obtained from htop
  return (memtotal - memfree - (buffers + cached + sreclaimable - shmem)) /
         (memtotal + 1e-6);  // added a small number to avoid zero-division
}

// (DONE) TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  long uptimeSec = 0;
  string line;

  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptimeSec;
  }

  return uptimeSec;
}

// Reference used for calculating jiffies
// https://knowledge.udacity.com/questions/129844
// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
// https://man7.org/linux/man-pages/man5/proc.5.html

// TODO: Read and return the number of jiffies for the system
// Currently not being used
long LinuxParser::Jiffies() {
  return LinuxParser::IdleJiffies() + LinuxParser::ActiveJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  long activejiffies = 0;
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);

  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream linestream(line);

    // burn through the first 13 columns
    // as we are interested numbers in the 14-17th columns (utime, stime,
    // cutime, cstime) Is there a better way?
    string temp;
    long num;
    for (int i = 0; i < 13; i++) {
      linestream >> temp;
    }

    for (int i = 0; i < 4; i++) {
      linestream >> num;
      activejiffies += num;
    }
  }

  return activejiffies / sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  // According to
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  // guess and guess_nice have already been included in user, and therefore we
  // don't need them
  // Could have mapped the string to the CPUStates, but I like my solution
  // better
  string cpu = "";
  long user, nice, system, idle, iowait, irq, softirq, steal;
  user = nice = system = idle = iowait = irq = softirq = steal = 0;

  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream linestream(line);

    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >>
        steal;
  }

  // excluding the idle jiffies
  return (user + nice + system + irq + softirq +
          steal);  // sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  // pretty much identical to LinuxParser::ActiveJiffies, except for
  // the components being added. Would be better to merge the two into one
  // function using passed references (as arguments) to obtain idle and active
  // jiffies
  string cpu = "";
  long user, nice, system, idle, iowait, irq, softirq, steal;
  user = nice = system = idle = iowait = irq = softirq = steal = 0;

  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream linestream(line);

    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >>
        steal;
  }

  // excluding the non-idle jiffies
  return (idle + iowait);  //  sysconf(_SC_CLK_TCK);
}

// TODO: Read and return CPU utilization
vector<string>  LinuxParser::CpuUtilization() {
  // Get a list of process ids
  vector<int> process_ids = LinuxParser::Pids();

  // Compute the CPU load for every process
  vector<string> utilization;
  for (int pid : process_ids) {

    // CPU load
    long uptime = LinuxParser::UpTime(pid);
    float total_time = (float)LinuxParser::ActiveJiffies(pid);
    float elapsed = (float)uptime;
    // string usage = std::to_string((total_time / elapsed));

    // other attributes
    string command = LinuxParser::Command(pid);
    string ram = LinuxParser::Ram(pid);
    string user = LinuxParser::User(pid);

    // NOTE:
    // On my computer, there are pids that couldn't be found, yet picked up by LinuxParser::Pids()
    // Checked the /proc directory and htop and indeed some of the pids don't exist.
    // A workaround is to check if a user of the pid can be found, if not, the process is not going to 
    // be created.
    if (user != "NOTFOUND" && command != "" && ram != "")
      utilization.push_back(std::to_string(pid) + "\n" + std::to_string(total_time) + "\n" + 
                          std::to_string(elapsed) + "\n" + command + "\n" + ram + "\n" + 
                          std::to_string(uptime) + "\n" + user);
  }

  return utilization;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int totalprocesses = 0;
  string line, str;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> str;

      if (str == "processes") {
        linestream >> totalprocesses;
        break;
      }
    }
  }

  return totalprocesses;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int procsrunning = 0;
  string line, str;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> str;

      if (str == "procs_running") {
        linestream >> procsrunning;
        break;
      }
    }
  }

  return procsrunning;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string cmd;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    getline(stream, cmd);
  }

  return cmd;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string ram;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    string line;
    while (getline(stream, line)) {
      string str;
      std::istringstream linestream(line);
      linestream >> str;

      if (str == "VmSize:") {
        linestream >> ram;
        break;
      }
    }
  }

  return ram;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string uid;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    string line;
    while (getline(stream, line)) {
      string str;
      std::istringstream linestream(line);
      linestream >> str;
      if (str == "Uid:") {
        linestream >> uid;
        break;
      }
    }
  }

  return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
// NOTE: modified signature! ( the argument name is changed to uid)
// It makes more sense to use uid as we are finding the corresponding username
// for the uid
string LinuxParser::User(int pid) {
  // Tokenize the string using a custom separator
  // Modified from:
  // https://www.geeksforgeeks.org/how-to-split-a-string-in-cc-python-and-java/
  // to extract two substrings (uid and user)

  string uid = LinuxParser::Uid(pid);

  string user;
  const string sep = ":";
  bool found = false;

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    string line;
    while (getline(stream, line)) {
      int start = 0;
      int end = line.find(sep);

      for (int pos = 0; pos < 3; pos++) {
        if (pos == 0) 
          user = line.substr(start, end - start);
        // if there is a match!
        else if (pos == 2 && uid == line.substr(start, end - start))
          found = true;
        
        start = end + sep.size();
        end = line.find(sep, start);
      }
      if (found) break;
    }
  }

  if (!found)
    return "NOTFOUND";
  else
    return user;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  long starttime = 0;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    string line;
    getline(stream, line);
    std::istringstream linestream(line);

    // burn through the first 21 columns
    string temp;
    for (int i = 0; i < 21; i++) linestream >> temp;
    linestream >> starttime;
  }

  // Total elapsed time since the process started
  long seconds =  LinuxParser::UpTime()  - starttime / sysconf(_SC_CLK_TCK); // LinuxParser::UpTime() * sysconf(_SC_CLK_TCK) - starttime;  //   sysconf(_SC_CLK_TCK);

  return seconds;
}
