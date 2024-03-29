#ifndef PROCESS_H
#define PROCESS_H

// Defines a Process
typedef struct Process
{
  int priority;
  int endTime;
  int startTime;
  int frontCPUq, frontIOq;
  int backCPUq, backIOq;
  int sizeCPUq;
  int sizeIOq;
  int *CPUq;
  int *IOq;
  int status; // 0=CPU, 1=IO, 2=finished
} Process;

void addCPUToProcess(Process *process, int cpuTime);
void addIOToProcess(Process *process, int ioTime);
Process createProcess(int priority, int startTime);

// Add a CPU process to queue
void addCPUToProcess(Process *process, int cpuTime)
{
  // If there is no more room in the array
  if (process->backCPUq == process->sizeCPUq)
  {
    // Double the size of the array, and exit with failure if allocation fails
    process->sizeCPUq *= 2;
    process->CPUq = realloc(process->CPUq, process->sizeCPUq * sizeof(int));
    if (process->CPUq == NULL)
    {
      exit(EXIT_FAILURE);
    }
  }

  // Add the CPU time to the array, increment the back of q
  process->CPUq[process->backCPUq] = cpuTime;
  process->backCPUq++;
  return;
}

// Add an IO request to queue
void addIOToProcess(Process *process, int ioTime)
{
  // If there is no more room in the array
  if (process->backIOq == process->sizeIOq)
  {
    // Double the size of the array, and exit with failure if allocation fails
    process->sizeIOq *= 2;
    process->IOq = realloc(process->IOq, process->sizeIOq * sizeof(int));
    if (process->IOq == NULL)
    {
      exit(EXIT_FAILURE);
    }
  }

  // Add the IO time to the array, increment the back of q
  process->IOq[process->backIOq] = ioTime;
  process->backIOq++;
  return;
}

// Create a process with certain priority and starting time
Process createProcess(int priority, int startTime)
{
  Process newProcess;
  newProcess.priority = priority;
  newProcess.startTime = startTime;
  newProcess.sizeCPUq = 20;
  newProcess.sizeIOq = 20;
  newProcess.endTime = 0;
  newProcess.status = 0;

  newProcess.CPUq = malloc(newProcess.sizeCPUq * sizeof(int));
  newProcess.IOq = malloc(newProcess.sizeIOq * sizeof(int));

  if (newProcess.CPUq == NULL || newProcess.IOq == NULL)
  {
    printf("Failed to allocate memory for new process\n");
    exit(EXIT_FAILURE);
  }

  newProcess.frontCPUq = 0;
  newProcess.frontIOq = 0;
  newProcess.backCPUq = 0;
  newProcess.backIOq = 0;

  return newProcess;
}

#endif /* PROCESS_H */