


						/*  Application/Process LoadBalancer System Software */
						
/*LoadBalancing system software is intended to work within a server thereby 
increasing the CPU performance by equally sharing the load among all the cores
of the server. The load balancer will handle all the requests coming in to that
particular server and optimally assign the processes to the cores available in 
the server.*/

#define _GNU_SOURCE
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/sysinfo.h>
#include<assert.h>
#include<sched.h>
#define threshold 30

/* 
Structure 'process' to hold the process id, core id and CPU utilization value 
of each process 
*/

struct process
{
   int process_id;
   int core_id;
   float cpu_util;
};

/*
	Function name : initialize_process_info() 
	Purpose       : To initialize the process information of all the currently 
                  running processes
	Inputs        : Structure array address, number of cores
	Output        : Process information of all the processes are stored in the 
                  respective structures
*/

void initialize_process_info(struct process *p,int processors)
{
  FILE *outputfile,*fp;
  char var[80];
  // Opens the file to read the contents and store the process information
  fp = fopen("process_info.txt","r");       
  if(fp == NULL)
  { 
    printf("Error in opening file!!");
    exit(0);
  }
  else
  {  
    char s[80];
    int i =0,k=0,id=0,c=0;
    fgets(s,80,fp);
    // Iterates through each line to get the process_id of all running processes
    while(fgets(s,80,fp))                
    {  
        while(s[k]==' ') { k++; }
        /*
        process_id starts from third character in each line 
        (Default postion in the file)
        */
        for(int j=k;s[j]!=' ';j++)        
        {
          id*=10;
          id+= s[j]-'0';                   // Converts char to int
          k=j; // Keeps track of the last index in which the process_id exists                            
        }
        
        p[i].process_id = id;      // Assigns the process_id for the processes
        id=0;
        k+=10;
        
        while(s[k]==' '){  k++; }         // Iterates through the white spaces
        
        while(s[k]!='.')
        {
          c*=10;
          c+=s[k]-'0';
          k++;
        }
        // Includes the decimal point
        k++;
        c*=10;
        c+=s[k]-'0';
        // Assigns the CPU Utilization of the core the process runs on 
        p[i].cpu_util = c/10.0;           
        // Iterates to the end of line to get the core_id of that  process
        while(s[k]!='\n'){  k++; }        
        p[i].core_id = s[k-1]-'0';        // Assigns the core_id for the process
        i++;               // Increments i to store the next Process information
        k=0;
        c=0;       // Initializes the cpu utilization count for the next process
    }
  }
  fclose(fp);
}

/*
	Function Name : give_process_count()
	Purpose       : To find the number of currently running processes
	Input         : -
	Output        : number of currently running process
*/

int give_process_count()
{
  int count=0;                 //Keeps track of the number of running processes
  FILE *fp,*outputfile;
  char var[80];           
  //Executes the command and writes the contents into a file  "process_info.txt"                             
  fp = popen("ps -o pid,user,pcpu,psr axr", "r");     
  outputfile = fopen("process_info.txt", "w");
  while (fgets(var, sizeof(var), fp) != NULL) 
  {
        
    fprintf(outputfile,"%s",var);
    count++;
  }
  pclose(fp);                                         // Closes the pipe
  fclose(outputfile);
  return count;
}

/*	
	Function Name  : find_process_id()
	Purpose        : To find the process with maximum utilization value within 
                   a specific core
	Inputs         : core id in which the process has to be found, Structure array
                   address
	Output         : Index of the process with maximum utilization value within 
                   the specified core
*/

int find_process_id(int id,struct process *p)
{  
  float mx=-1;                   // Keeps track of the maximum utilization value
  int index;
  for(int i=0;i<give_process_count();i++)             
  {
    if(p[i].core_id == id)
    {
      if(mx < p[i].cpu_util)
      {
        index = i;
      }
    }
  }
  printf("\t\t\t\tWithin core %d, the process with maximum utilization value 
  (%0.2f) is %d\n",p[index].core_id, p[index].cpu_util,p[index].process_id);
  return index;
}

/*
	Function Name  : get_affinity()
	Purpose        : To find the least utilized core and to shift the process with
                   maximum utilization to that core
	Inputs         : Index of the process with maximum utilization value, number 
                   of cores,CPU-core utilization values, structure array address
	Output         : Process information of all the processes is stored in the 
                   respective structures
*/

void get_affinity(int p_1,int processors,float *cpu,struct process *p)
{ 
  int flag=-1,int_flag=-1;
  cpu_set_t mask;
  float min_core_util =cpu[0];     
  int min_core_id =-1;                                            
  while(flag!=0)
  {                                      // waits until the process finds a core
    for(int j =0;j<processors;j++)
    {
        if(cpu[j]+p[p_1].cpu_util < threshold)
        {  
          if(min_core_util+p[p_1].cpu_util+cpu[j]  > cpu[j]+p[p_1].cpu_util)
          {
            min_core_id =j;
            int_flag=1;
          }                    
        }
    }
        if( int_flag == 1)
        {
          CPU_ZERO(&mask);              // Removes all the CPUs from the CPU set
          CPU_SET(min_core_id, &mask);             // Assings CPU to the process
          flag=0; 
          p[p_1].core_id = min_core_id;
          printf("\t\t\t\tLeast Utilized core: %d\n\n",min_core_id); 
          printf("\t\t\t\t*************************************************\n");
          printf("\n\t\t\t\tThe Process %d has been shifted to the core %d\n\n",
          p[p_1].process_id,min_core_id);  
          printf("\t\t\t\t********************************
                  *****************\n\n");         
        }
        if (flag !=0) 
        {
          //printf("\t\t\t\tNo free cores!\n\n");
          find_core_util(processors,cpu); printf("\n");        
          min_core_util =cpu[0];
        }
  }
}

/* 
	Function Name : find_core_util()
	Purpose       : To find the utilization of every core in the system
	Inputs        : number of processes, CPU-core utilization values of all cores
	Output        : updates the CPU-core utilization value of all the cores
*/

void find_core_util(int count, float *cpu)
{
  char arr[50];
  char arr1[50];
  FILE *fp;
  char var[50];

  sleep(3);
  system("clear");
  printf("\t\t\t\t\t\t********************************************************
          **************\n\n");
  printf("\t\t\t\t\t\t\t\tApplication/Process Load Balancer\n");
  printf("\t\t\t\t\t\t\t\t---------------------------------\n");

  int processors= get_nprocs();                      // Gets the number of cores
  printf("\n\n\t\t\t\t\t\t\t\t\tNumber of Cores: %d\n",processors);
  printf("\t\t\t\t\tThreshold (configurable value which acts as an upper bound
           for the core utilization) : %d\n\n",threshold);
  printf("\t\t\t\t\t\t****************************************************
          *******************\n\n");
  printf("\t\t\t\t");
  for(int i=0;i<processors;i++)
  printf("CPU: %d\t\t",i);
  printf("\n\n\t\t\t\t");

  for(int i =0 ;i< count; i++)
  {
    strcpy(arr, "top 1 -n 1 | grep Cpu");
    strcpy(arr1, " | cut -c27-32");
    char ch=i+'0';
    strncat(arr,&ch,1);
    strcat(arr,arr1);
    fp = popen(arr, "r");
    while(fgets(var,sizeof(var),fp) != NULL)
    cpu[i]= atof(var);
    printf("%0.2f\t\t",cpu[i]);
    pclose(fp);
  }
  printf("\n");
}
 
int main()                                         
{

  int processors= get_nprocs();    
  float cpu[processors];
  int count=give_process_count();   
            
  struct process p[count]; 
  // Creates process structure to hold the process information
  initialize_process_info(p,processors);           
  find_core_util(processors,cpu); printf("\n");                      
  while(1)
  {

    int p_1;

    for(int i =0;i<processors;i++)                      // Does load balancing               
    {
        if(cpu[i]>threshold)
        {  
        
          printf("\n\t\t\t\tCore %d has exceeded the threshold of %d ! \n"
                  ,i,threshold);
          // p_1 stores the index returned from  find_process_id()
          p_1 = find_process_id(i,p);                 
          if(p[p_1].cpu_util != 0 && p[p_1].process_id >50)
          {
            printf("\t\t\t\t");
            printf("The Process %d has the maximum utilization value of %0.2f\n"
                   , p[p_1].process_id, p[p_1].cpu_util);   
            get_affinity(p_1,processors,cpu,p);        
          }
        }
        find_core_util(processors,cpu); printf("\n");         
    }

    count=give_process_count();
    struct process p[count];
    initialize_process_info(p,processors);    
    find_core_util(processors,cpu);   
  }
  return 0;
}
