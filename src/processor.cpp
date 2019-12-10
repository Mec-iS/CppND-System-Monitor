#include <vector>
#include <string>

#include "processor.h"
#include "linux_parser.h"

using std::vector;
using std::string;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  vector<string> cpu_stat = LinuxParser::CpuUtilization();
  // https://stackoverflow.com/a/23376195
  // Idle = idle + iowait
  // NonIdle = user + nice + system + irq + softirq + steal
  // Total = Idle + NonIdle
  float idle = std::stof(cpu_stat[3]) + std::stof(cpu_stat[4]);
  float non_idle = std::stof(cpu_stat[0]) + std::stof(cpu_stat[1]) +
    std::stof(cpu_stat[2]);
  float total = idle + non_idle;
  return non_idle / total;
}
