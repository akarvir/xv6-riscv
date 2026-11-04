#ifndef XV6_PROCSTATS_H
#define XV6_PROCSTATS_H

// Durations in RISC-V time-counter units.
struct procstats {
  uint64 turnaround;
  uint64 response;
  uint64 running;
  uint64 runnable;
  uint64 sleeping;
};

#endif
