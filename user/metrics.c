#include "kernel/types.h"
#include "kernel/procstats.h"
#include "user/user.h"

#define WORKERS 3

int
main(void)
{
  int sleeper = -1;
  int failed = 0;
  struct procstats stats;

  for (int i = 0; i <= WORKERS; i++) {
    int pid = fork();
    if (pid < 0) {
      fprintf(2, "metrics: fork failed\n");
      exit(1);
    }
    if (pid == 0) {
      if (i == WORKERS) {
        pause(15);
      } else {
        for (volatile uint j = 0; j < 30000000; j++)
          ;
      }
      exit(0);
    }
    if (i == WORKERS)
      sleeper = pid;
  }

  printf("Times in RISC-V time-counter units\n");
  printf("PID  Workload  Turnaround  Response  Running  Runnable  Sleeping\n");
  for (int i = 0; i <= WORKERS; i++) {
    int pid = waitstats(&stats);
    if (pid < 0) {
      fprintf(2, "metrics: waitstats failed\n");
      exit(1);
    }
    printf("%d  %s  %lu  %lu  %lu  %lu  %lu\n",
           pid, pid == sleeper ? "sleep" : "cpu", stats.turnaround,
           stats.response, stats.running, stats.runnable, stats.sleeping);
    if (stats.turnaround < stats.running + stats.runnable + stats.sleeping ||
        stats.response > stats.turnaround || stats.running == 0 ||
        (pid == sleeper && stats.sleeping == 0))
      failed = 1;
  }
  if (waitstats(&stats) != -1)
    failed = 1;
  printf("Metrics check: %s\n", failed ? "FAIL" : "PASS");
  exit(failed);
}
