/*
 *  The first in, first out algorithm for a scheduler
 *  Troy Harrison, Philip Oetinger
 *
 */

#include "util.h"
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_PROCESSES 1000

// A process contains its start/end time, list for its needed CPU and IO time
// and the remaining time until it becomes active
typedef struct Process {
  int timeToInsert;
  Vec cpuList;
  Vec ioList;

  size_t cpuHead;
  size_t ioHead;

  // For statistics
  int start;
  int end;
} Process;

// A scheduler, that contains the processes both active and inactive.
// It will manage the cpu and I/O device
typedef struct Scheduler {
  size_t currentTime;
  size_t numProcesses;
  Process *allProcesses;
} Scheduler;

// Create a scheduler
Scheduler newScheduler() {
  // Initialize variables
  Scheduler scheduler;
  scheduler.currentTime = 0;
  scheduler.numProcesses = 0;
  scheduler.allProcesses = calloc(MAX_PROCESSES, sizeof(Process));

  // Create a new vec for each process, can be changed later for dynamic
  for (size_t i = 0; i < MAX_PROCESSES; ++i) {
    scheduler.allProcesses[i].cpuList = newVec();
    scheduler.allProcesses[i].ioList = newVec();
  }

  Process *currentProcess = &scheduler.allProcesses[0];
  int startTime;
  while (scanf("%d", &startTime) == 1) {
    // Read in the details of each process, until EOF is found
    int priority = 0;
    scanf("%d", &priority);

    currentProcess->cpuHead = 0;
    currentProcess->ioHead = 0;
    currentProcess->timeToInsert = startTime;
    currentProcess->start = startTime;

    int cpuIn, ioIn;
    while (true) {
      scanf("%d ", &cpuIn);
      if (cpuIn == -1) {
        break;
      }
      scanf("%d", &ioIn);
      push(&currentProcess->cpuList, cpuIn);
      push(&currentProcess->ioList, ioIn);
    }
    scheduler.numProcesses++;
    currentProcess++;
  }
  return scheduler;
}

// Util for printing processes
void printProcess(Process *process) {
  printf("CPU: ");
  printVec(&process->cpuList);
  printf("IO: ");
  printVec(&process->ioList);
  printf("\n");
}

// Print a scheduler
void printScheduler(Scheduler *scheduler) {
  printf("Time: %lu, NumProcesses: %lu\n", scheduler->currentTime,
         scheduler->numProcesses);
  for (int i = 0; i < scheduler->numProcesses; i++) {
    printProcess(&scheduler->allProcesses[i]);
  }
}

// Free the memory of a scheduler
void freeScheduler(Scheduler *scheduler) {
  for (size_t i = 0; i < MAX_PROCESSES; ++i) {
    freeVec(&scheduler->allProcesses[i].cpuList);
    freeVec(&scheduler->allProcesses[i].ioList);
  }
  free(scheduler->allProcesses);
}

/* SYSTEM:
TODO Get rid of MAX_PROCESSES.
{timeToInsert, [cpus], [ios], start, end}


batch  : -4:{3,2,1,3},{4,5,6,7} 0,0
         0{5,5,5,5},{0,5,5,5} 0,0
         3:{5,5,5,5},{5,5,5,5} 0,0

current time: 9
io:{}
active:{0}

Steps:
Look at batch, find something to activate
Look at IO jumps
Look at active jumps
  Compare with new activations

Inser active at 0
repeat
*/

// Checks if there is still work to do within a process
bool isProcessDone(Process *process) {
  if (process->cpuHead == process->cpuList.len) {
    return false;
  }
  if (process->ioHead == process->ioList.len) {
    return false;
  }

  return true;
}

// Check if all processes are finished
bool isSchedulerDone(Scheduler *scheduler) {
  for (int i = 0; i < scheduler->numProcesses; ++i) {
    Process *process = &scheduler->allProcesses[i];
    if (process->timeToInsert > 0)
      return false;
    if (!isProcessDone(process))
      return false;
  }
  return true;
}

// Check if any processes want to join the fun
void moveInactiveProcessToActive(Scheduler *scheduler, Vec *vec) {
  for (int i = 0; i < scheduler->numProcesses; i++) {
    Process *process = &scheduler->allProcesses[i];
    if (process->timeToInsert == 0) {
      push(vec, i);
      process->start = scheduler->currentTime;
    }
  }
}

// Find the lowest amount of time between the IO, CPU, and inactive times to
// find out what will finish first, to then reduce all others by the delta
int findLowestDelta(Scheduler *scheduler, Vec *active, Vec *io) {
  int delta = INT_MAX;
  for (size_t i = 0; i < scheduler->numProcesses; ++i) {
    int processTime = scheduler->allProcesses[i].timeToInsert;
    if (processTime > 0) {
      delta = min(delta, processTime);
    }
  }
  if (active->len > 0) {
    min(delta, peek(active));
  }
  if (io->len > 0) {
    min(delta, peek(io));
  }

  return delta;
}

// Take away the delta from all active things (io, active, and timeToStart from
// standby)
void reduceByDelta(Scheduler *scheduler, Vec *active, Vec *io, int delta) {
  for (size_t i = 0; i < scheduler->numProcesses; ++i) {
    scheduler->allProcesses[i].timeToInsert -= delta;
  }

  if (active->len > 0) {
    Vec *cpu = &scheduler->allProcesses[active->data[0]].cpuList;
    for (size_t i = 0; i < cpu->len; ++i) {
      if (cpu->data[i] != 0) {
        cpu->data[i] -= delta;
        break;
      }
    }
  }

  if (io->len > 0) {
    Vec *cpu = &scheduler->allProcesses[io->data[0]].cpuList;
    for (size_t i = 0; i < cpu->len; ++i) {
      if (cpu->data[i] != 0) {
        cpu->data[i] -= delta;
        break;
      }
    }
  }
}

void handleFinishedActive(Scheduler *scheduler, Vec *active, Vec *io) {
  if (active->len > 0) {
    Process *process = &scheduler->allProcesses[active->data[0]];
    if (isProcessDone(process)) {
      pop(active);
    } else if (process->cpuHead < process->cpuList.len &&
               process->cpuList.data[process->cpuHead] == 0) {
      process->cpuHead++;
      push(io, pop(active));
    }
  }
}

void handleFinishedIo(Scheduler *scheduler, Vec *active, Vec *io) {
  if (active->len > 0) {
    Process *process = &scheduler->allProcesses[active->data[0]];
    if (isProcessDone(process)) {
      pop(io);
    } else if (process->ioHead < process->ioList.len &&
               process->ioList.data[process->ioHead] == 0) {
      process->ioHead++;
      push(active, pop(io));
    }
  }
}

// The main loop of the scheduler
void runScheduler(Scheduler *scheduler) {
  Vec io = newVec();
  Vec active = newVec();

  while (!isSchedulerDone(scheduler)) {
    printScheduler(scheduler);
    moveInactiveProcessToActive(scheduler, &active);
    int delta = findLowestDelta(scheduler, &active, &io);
    reduceByDelta(scheduler, &active, &io, delta);
    handleFinishedActive(scheduler, &active, &io);
    handleFinishedIo(scheduler, &active, &io);
  }

  printf("Scheduler Shutting Down...\n");
}

int main() {
  // Scheduler with time at 0 and all the processes loaded in the batch.
  Scheduler scheduler = newScheduler();
  printScheduler(&scheduler); // debug
  runScheduler(&scheduler);   // Begin running

  freeScheduler(
      &scheduler); // Free before shutdown to appease the valgrind gods

  return EXIT_SUCCESS;
}
