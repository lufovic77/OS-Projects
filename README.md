# OS-Projects

## 1. Project Introduction
    In this project, we will see how processes are managed in Linux and make a system call to 
log the information of the current running processes. Your task for this project is to make your 
own system call to enable the given application sysps to show the information of  the current 
running processes. 

----

## 2. Implementations
### 2.1 Output
The user application sysps will be provided to print the following.
- UID: user ID (root, $USER)
- PID: process ID
- PPID: process ID of the parent process
- NICE: nice value of the current process (-20 ~ 19)
- STATE: current state of the process (S: sleeping, R: running, Z: zombie)
- CPU#: CPU # of where the process is scheduled
- CMD: the executable name of the process
- nr_switches: # of context switches the processes made
- nr_running: # of running processes in the runqueue where the process is in
- list of siblings: information of the process’s siblings
- list of children: information of the process’s children
- range of VMA: the start and end addresses of each virtual memory address (CODE, DATA,
HEAP, STACK*)
- number of VMA: the number of the virtual memory address



