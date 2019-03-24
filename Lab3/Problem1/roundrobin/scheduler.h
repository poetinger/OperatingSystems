#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdlib.h>

#include "process.h"

typedef struct Scheduler
{
  int quantum;
  int numP1, numP2, numP3;
  int sizeP1, sizeP2, sizeP3;
  Process *priority1;
  Process *priority2;
  Process *priority3;
} Scheduler;

Scheduler createScheduler(int quantum);
void addProcessToSchedulerP1(Scheduler *scheduler, Process *process);

Scheduler createScheduler(int quantum)
{
  Scheduler newScheduler;
  newScheduler.quantum = quantum;

  newScheduler.sizeP1 = 20;
  newScheduler.sizeP2 = 20;
  newScheduler.sizeP3 = 20;
  newScheduler.numP1 = 0;
  newScheduler.numP2 = 0;
  newScheduler.numP3 = 0;

  newScheduler.priority1 = malloc(newScheduler.sizeP1 * sizeof(Process));
  newScheduler.priority2 = malloc(newScheduler.sizeP2 * sizeof(Process));
  newScheduler.priority3 = malloc(newScheduler.sizeP3 * sizeof(Process));

  return newScheduler;
}

void addProcessToSchedulerP1(Scheduler *scheduler, Process *process)
{
  if (scheduler->numP1 == scheduler->sizeP1)
  {
    scheduler->sizeP1 *= 2;
    scheduler->priority1 = realloc(scheduler->priority1, scheduler->sizeP1 * sizeof(Process));
  }
  scheduler->priority1[scheduler->numP1] = *process;
  scheduler->numP1++;
}

void setupScheduler(Scheduler Scheduler)
{
  char *line = NULL;
  size_t len = 0;

  while (getline(&line, &len, stdin) != -1)
  {
    int priority;

    printf(line);
    sscanf(line, "%d ", &priority);
    printf("Adding a process, priority %d...\n", priority);
    printf(line);
    Process newProcess = createProcess(priority);
  }

  free(line);
}

#endif /* SCHEDULER_H */