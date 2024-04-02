-- Running the Program --
Running "make all" will create the executable Scheduler file. From there, running
"./Scheduler (input file name)" will start the program.

-- Methods --
*readProcesses*: Reads the input file, allocates memory for each process, and creates each process with its defined
characteristics (ID, priority, etc.).

*calculateMetrics*: Calculates the turnaround time, waiting time, and throughput for each process, as well as
providing the print statements for the terminal.

*compareArrivalTime*: Compares the arrival times for use in sorting the processes in the FCFS method.

*updatePriorityWaitingTime*: Helper method for Priority Scheduling

*FCFS*: First-Come-First-Served scheduler. Uses compareArrivalTime to sort the processes by which came first

*updateWaitingTime*: Helper method for SJF

*SJF*: Shortest-Job-First scheduler. Uses updateWaitingTime to determine which processes have the shortest
waiting time

*priorityScheduling*: Priority scheduler.
