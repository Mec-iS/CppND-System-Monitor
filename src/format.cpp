#include <sstream>
#include <string>

#include "../include/format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int hours, minutes;
  minutes = seconds / 60;
  hours = minutes / 60;
  std::stringstream s;
  s << hours << ":" << int(minutes % 60) << ":" << int(seconds % 60)
    << std::endl;
  return s.str();
}
