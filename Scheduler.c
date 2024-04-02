#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct {
    int processID;
    int arrivalTime;
    int burstDuration;
    int priority;
    int waitingTime;
    int turnaroundTime;
    int remainingTime;
} Process;

int readProcesses(const char* filename, Process **processes) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return 0;
    }

    int count = 0;
    char buffer[128];

    // Count the number of lines
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (buffer[0] != '\n'){
            count++;
        }
    }

    if (count == 0) {
        fclose(file);
        return 0;
    }

    // Allocate memory for processes
    *processes = (Process *)malloc(count * sizeof(Process));
    if (*processes == NULL) {
        perror("Failed to allocate memory for processes");
        fclose(file);
        return 0;
    }

    rewind(file);

    // Iterate through the file
    int i = 0;
    while (i < count && fgets(buffer, sizeof(buffer), file) != NULL) {
        Process *proc = &((*processes)[i]);
        if (sscanf(buffer, "%d,%d,%d,%d", &proc->processID, &proc->arrivalTime, &proc->burstDuration, &proc->priority) == 4) {
            proc->waitingTime = 0;
            proc->turnaroundTime = 0;
            proc->remainingTime = proc->burstDuration; // For preemptive scheduling
            i++;
        }
    }

    fclose(file);

    return count;
}

void calculateMetrics(Process *processes, int count) {
    if (count <= 0) return;

    double totalTurnaroundTime = 0.0, totalWaitingTime = 0.0, totalTime = 0.0;
    for (int i = 0; i < count; i++) {
        totalTurnaroundTime += processes[i].turnaroundTime;
        totalWaitingTime += processes[i].waitingTime;
        if (processes[i].turnaroundTime + processes[i].arrivalTime > totalTime) {
            totalTime = processes[i].turnaroundTime + processes[i].arrivalTime;
        }
    }

    double averageTurnaroundTime = totalTurnaroundTime / count;
    double averageWaitingTime = totalWaitingTime / count;
    double throughput = count / totalTime;

    printf("Average Turnaround Time: %.3f\n", averageTurnaroundTime);
    printf("Average Waiting Time: %.3f\n", averageWaitingTime);
    printf("Throughput: %.3f\n", throughput);
}

int compareArrivalTime(const void *a, const void *b) {
    Process *p1 = (Process *)a;
    Process *p2 = (Process *)b;
    return p1->arrivalTime - p2->arrivalTime;
}

void updatePriorityWaitingTime(Process *processes, int n, int totalTime, int highestPriorityIndex) {
    for (int i = 0; i < n; i++) {
        if (i != highestPriorityIndex && processes[i].arrivalTime <= totalTime && processes[i].remainingTime > 0) {
            processes[i].waitingTime += 1;
        }
    }
}

void FCFS(Process *processes, int count) {
    // Sort processes by arrival time
    qsort(processes, count, sizeof(Process), compareArrivalTime);

    // First process always starts execution at arrival
    processes[0].waitingTime = 0;
    processes[0].turnaroundTime = processes[0].burstDuration;

    for (int i = 1; i < count; i++) {
        // Completion time of the previous process
        int completionTime = processes[i-1].arrivalTime + processes[i-1].turnaroundTime;

        // Current process starts after the previous has completed
        int start = completionTime > processes[i].arrivalTime ? completionTime : processes[i].arrivalTime;

        // Waiting time is the start time minus arrival time
        processes[i].waitingTime = start - processes[i].arrivalTime;

        // Turnaround time is waiting time plus burst duration
        processes[i].turnaroundTime = processes[i].waitingTime + processes[i].burstDuration;
    }
}

void updateWaitingTime(Process *processes, int n, int totalTime, int shortestIndex) {
    for (int i = 0; i < n; i++) {
        if (i != shortestIndex && processes[i].arrivalTime <= totalTime && processes[i].remainingTime > 0) {
            processes[i].waitingTime += 1;
        }
    }
}

void SJF(Process *processes, int count) {
    int completed = 0, currentTime = 0, shortest = 0, minTime = INT_MAX;
    int check = 0;

    while (completed != count) {
        // Find process with the shortest remaining time
        for (int i = 0; i < count; i++) {
            if ((processes[i].arrivalTime <= currentTime) && (processes[i].remainingTime < minTime) && processes[i].remainingTime > 0) {
                minTime = processes[i].remainingTime;
                shortest = i;
                check = 1;
            }
        }

        if (check == 0) {
            currentTime++;
            continue;
        }

        // Reduce remaining time of selected process
        processes[shortest].remainingTime--;
        minTime = processes[shortest].remainingTime;
        if (minTime == 0) minTime = INT_MAX;

        // Update waiting time for other processes
        updateWaitingTime(processes, count, currentTime, shortest);

        // If a process gets completely executed
        if (processes[shortest].remainingTime == 0) {
            completed++;
            check = 0;

            // Finish time for process
            int finishTime = currentTime + 1;
            // Calculate turnaround time: finishTime - arrivalTime
            processes[shortest].turnaroundTime = finishTime - processes[shortest].arrivalTime;
            // Waiting time is turnaround time minus burst time
            processes[shortest].waitingTime = processes[shortest].turnaroundTime - processes[shortest].burstDuration;
        }
        // Increment time
        currentTime++;
    }
}

void priorityScheduling(Process *processes, int count) {
    int completed = 0, currentTime = 0, highestPriority = 0, minPriority = INT_MAX;
    int check = 0;

    while (completed != count) {
        // Find process with the highest priority
        for (int i = 0; i < count; i++) {
            if ((processes[i].arrivalTime <= currentTime) && (processes[i].priority < minPriority) && processes[i].remainingTime > 0) {
                minPriority = processes[i].priority;
                highestPriority = i;
                check = 1;
            }
        }

        if (check == 0) {
            currentTime++;
            continue;
        }

        // Reduce remaining time of selected process
        processes[highestPriority].remainingTime--;
        minPriority = processes[highestPriority].remainingTime == 0 ? INT_MAX : processes[highestPriority].priority;

        // Update waiting time for other processes
        updatePriorityWaitingTime(processes, count, currentTime, highestPriority);

        // If a process gets completely executed
        if (processes[highestPriority].remainingTime == 0) {
            completed++;
            check = 0;

            // Finish time for process
            int finishTime = currentTime + 1;
            // Calculate turnaround time: finishTime - arrivalTime
            processes[highestPriority].turnaroundTime = finishTime - processes[highestPriority].arrivalTime;
            // Waiting time is turnaround time minus burst time
            processes[highestPriority].waitingTime = processes[highestPriority].turnaroundTime - processes[highestPriority].burstDuration;
        }
        // Increment time
        currentTime++;
    }
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Incorrect number of arguments. Usage: %s <input_file_name>. Exiting.\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    Process *processes = NULL;
    int count = readProcesses(filename, &processes);

    if (count > 0) {
        printf("--- FCFS ---\n");
        FCFS(processes, count);
        calculateMetrics(processes, count);

        // Reset waiting and turnaround times for the next scheduling
        for (int i = 0; i < count; i++) {
            processes[i].waitingTime = 0;
            processes[i].turnaroundTime = 0;
            processes[i].remainingTime = processes[i].burstDuration;
        }

        printf("\n--- SJFP ---\n");
        SJF(processes, count);
        calculateMetrics(processes, count);

        // Reset waiting and turnaround times for the next scheduling
        for (int i = 0; i < count; i++) {
            processes[i].waitingTime = 0;
            processes[i].turnaroundTime = 0;
            processes[i].remainingTime = processes[i].burstDuration;
        }

        printf("\n--- Priority ---\n");
        priorityScheduling(processes, count);
        calculateMetrics(processes, count);
    } else {
        printf("Failed to read processes.\n");
    }

    free(processes);
    return 0;
}
