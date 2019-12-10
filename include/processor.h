#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
   long Jiffies = 0;
   long ActiveJiffies = 0;
   long IdleJiffies = 0;
};

#endif
