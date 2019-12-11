#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() {
  return this->pid;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return 0; }

// TODO: Return the command that generated this process
string Process::Command() {
  return LinuxParser::Command(this->pid);
}

// TODO: Return this process's memory utilization
string Process::Ram() {
  // return LinuxParser::Ram(this->pid);

  string ram = LinuxParser::Ram(this->pid);
  if (ram == "0") {
    return ram;
  } else {
    int i = std::stoi(ram);
    i = i / 1000;
    string r = std::to_string(i);
    return r;
  }
}

// TODO: Return the user (name) that generated this process
string Process::User() {
  return LinuxParser::User(this->pid);
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() {
  return LinuxParser::UpTime(this->pid);
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process& a) {
  if (LinuxParser::RamInt(this->pid) <
      LinuxParser::RamInt(a.Pid())) {
      return true;
      }
  return false;

}
