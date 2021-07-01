# Application-Process-LoadBalancing-in-Servers
LoadBalancing system software is intended to work within a server thereby increasing the CPU performance by equally sharing the load among all the cores of the server.
The load balancer will handle all the requests coming in to that particular server and optimally assign the processes to the cores available in the server.

# Problem Statement
There can be multiple users who can generate multiple requests in a multicore system. As a result, the number of running processes increases.
These multiple processes lead to high CPU-core utilization, thereby affecting the overall performance of the system. The system may enter a hung state because of high CPU-core utilization (100%). 
Our requirement is to develop a LoadBalancing system software which will balance the load across all the available cores. 

# Solution
The LoadBalancing  system software continuously monitors the system for CPU-core overloading. Threshold is a configurable value which is used to check whether the CPU-core utilization is exceeded or not.
The running processes information are extracted.
The utilization value of all the cores are checked.
If process_utilization > threshold , then shift it to the core with minimum CPU-core utilization.
