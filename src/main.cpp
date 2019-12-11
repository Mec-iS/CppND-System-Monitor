#include <iostream>

#include "ncurses_display.h"
#include "system.h"
#include "process.h"

#include "linux_parser.h"

int main() {
  System system;
  NCursesDisplay::Display(system);

  // int i = LinuxParser::RunningProcesses();
  // std::cout << i;
}
