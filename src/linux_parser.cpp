#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <assert.h>
#include <unistd.h>  // used for clock ticks

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);  // file buffer
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);  // string buffer
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
  string os, kernel;
  string line;
  // regex for kernel version
  std::regex k_regex("^(\\d+\\.)?(\\d+\\.)?(\\*|\\d+)?(-(.*))$");

  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream) {
      linestream >> kernel;
      if (std::regex_match(kernel, k_regex)) {
        return kernel;
      }
    }
  }

  return "Cannot find kernel version";
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  // MemTotal:       16126660 kB
  // MemAvailable:   11431104 kB
  float total = 0.0;
  float available = 0.0;
  bool check_total = false;
  bool check_available = false;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream) {
        string key, value;
        linestream >> key >> value;
        if (key.rfind("MemTotal", 0) == 0) {
          // https://stackoverflow.com/a/40441240
          float v = std::stof(value);
          total = v;
          available += v;
          check_total = true;
        }
        if (key.rfind("MemAvailable", 0) == 0) {
          // https://stackoverflow.com/a/40441240
          available -= std::stof(value);
          check_available = true;
        }
        if (check_total && check_available) {
          return (available / total);
        }
      }
    }
  }
  return 0;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  // return system uptime (not process)
  string line;
  string uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;

    // convert string to long double
    try {
      long value = std::stold(uptime);
      return value;
    } catch (...) {
      throw "Error";
    }
  }
  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
   vector<string> cpu_stat = LinuxParser::CpuUtilization();
   float idle = std::stof(cpu_stat[3]) + std::stof(cpu_stat[4]);
   float non_idle = std::stof(cpu_stat[0]) + std::stof(cpu_stat[1]) +
     std::stof(cpu_stat[2]);
   float total = idle + non_idle;
   return total;
}

// Read token at position inside a string (divided by whitespace)
string LinuxParser::readLineToken(string line, int position) {
  // use iterators to scan the line and return the
  // requested token at position
  std::istringstream iss(line);
  vector<string> results(
     std::istream_iterator<string>{iss},
     std::istream_iterator<string>()
   );
  return results[position];
}

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string p = std::to_string(pid);
  std::ifstream stream(kProcDirectory + p + kStatFilename);
  if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
      string jiffies = readLineToken(line, 21);
      return std::stoi(jiffies);
    }
  }
  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> cpu_stat = LinuxParser::CpuUtilization();
  float non_idle = std::stof(cpu_stat[0]) + std::stof(cpu_stat[1]) +
    std::stof(cpu_stat[2]);
  return non_idle;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu_stat = LinuxParser::CpuUtilization();
  float idle = std::stof(cpu_stat[3]) + std::stof(cpu_stat[4]);
  return idle;

}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  // https://rosettacode.org/wiki/Linux_CPU_utilization
  std::ifstream stream(kProcDirectory + kStatFilename);
  vector<string> times;
  if (stream.is_open()) {
    // read the first line
    string line;
    while (std::getline(stream, line)) {
      // https://stackoverflow.com/a/23376195
      //     user    nice   system  idle      iowait ...
      //cpu  74608   2520   24433   1117073   6176   ...
      std::istringstream linestream(line);
      string _;
      linestream >> _; // exclude first word in line
      while (linestream) {
        string jiffies;
        linestream >> jiffies;
        times.push_back(jiffies);
        if (times.size() == 5) { break; }
      }
      return times;
    }
  }
  throw "Error";
}

// Read cpu utilization of a process
float LinuxParser::CpuUtilization(int pid) {
  string p = std::to_string(pid);
  std::ifstream stream(kProcDirectory + p + kStatFilename);
  if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
      // https://stackoverflow.com/a/16736599
      float total_time = (std::stoi(readLineToken(line, 13))
                        + std::stoi(readLineToken(line, 14))
                        + std::stoi(readLineToken(line, 15))
                        + std::stoi(readLineToken(line, 16))) / sysconf(_SC_CLK_TCK);

      float elapsed_time =  LinuxParser::UpTime() -
             (std::stoi(readLineToken(line, 21)) / sysconf(_SC_CLK_TCK));

      return total_time / elapsed_time;
    }
  }
  return 0;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream) {
        string key, value;
        linestream >> key >> value;
        if (key.rfind("processes", 0) == 0) {
          // https://stackoverflow.com/a/40441240
          int i = std::stoi(value);
          return i;
        }
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream) {
        string key, value;
        linestream >> key >> value;
        if (key.rfind("procs_running", 0) == 0) {
          // https://stackoverflow.com/a/40441240
          int i = std::stoi(value);
          return i;
        }
      }
    }
  }
  return 1;
}

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string p = std::to_string(pid);
  std::ifstream stream(kProcDirectory + p + kCmdlineFilename);
  if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
      return line;
    }
  }
  return "No command";
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string p = std::to_string(pid);
  std::ifstream stream(kProcDirectory + p + kStatusFilename);
  if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream) {
        string key, value;
        linestream >> key >> value;
        if (key.rfind("VmSize", 0) == 0) {
          // https://stackoverflow.com/a/40441240
          return value;
        }
      }
    }
  }
  return "0";
}

// Read and convert RAM string to RAM integer
int LinuxParser::RamInt(int pid) {
  return std::stoi(LinuxParser::Ram(pid));
}

// TODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string p = std::to_string(pid);
  std::ifstream stream(kProcDirectory + p + kStatusFilename);
  if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream) {
        string key, value;
        linestream >> key >> value;
        if (key.rfind("Uid", 0) == 0) {
          // https://stackoverflow.com/a/40441240
          return value;
        }
      }
    }
  }
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  return LinuxParser::Uid(pid);
}

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  return LinuxParser::UpTime() - (LinuxParser::ActiveJiffies(pid) / sysconf(_SC_CLK_TCK));
}
