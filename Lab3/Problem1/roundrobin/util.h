#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include "process.h"
#include "scheduler.h"

void printArrays(int *array, int length)
{
    printf("[");
    for (int i = 0; i < length - 1; i++)
    {
        printf("%d,", array[i]);
    }
    printf("%d]", array[length - 1]);
}

void printProcess(Process testProcess)
{
    printf("Process CPUq: ");
    printArrays(testProcess.CPUq, testProcess.sizeCPUq);
    printf("\nProcess IOq: ");
    printArrays(testProcess.IOq, testProcess.sizeIOq);
    printf("\n");
}

void printScheduler(Scheduler scheduler)
{
    printf("Scheduler (Q=%d) Contains:\nPriority 1:\n", scheduler.quantum);
    Process process;
    for (int i = 0; i < scheduler.numP1; i++)
    {
        process = scheduler.priority1[i];
        printProcess(process);
    }
}

#endif /* UTIL_H */