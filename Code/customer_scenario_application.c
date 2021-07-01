#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<time.h>

void delay(int number_of_seconds)
{
	// Converting time into milli_seconds
	int milli_seconds = 1000 * number_of_seconds;

	// Storing start time
	clock_t start_time = clock();

	// looping till required time is not achieved
	while (clock() < start_time + milli_seconds);
}

int main()
{ 
    clock_t before = clock();
    int duration=0;
	
    for(int i=0;i<10;i++)
    {
    	if(fork() == 0)
    	{
    		printf("Customer application %d started\n",getpid());
    		duration+=2000;
    		delay(duration);
    	}
    }
    return 0;
}
