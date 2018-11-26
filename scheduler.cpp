#include "scheduler.h"

bool cpuBusy = false;

int totalWait = 0;
int processesStarted = 0; //if a job is blocked, it counts twice.
int maxWait = 0;

//Simply used for statistics reporting
double getAvgWait()
{
	return (double) totalWait / processesStarted;
}

//Routine triggered once per quantum (on simulated clock interrupt) to determine which
//process gets CPU time and maintains queues
int scheduler()
{
	int i;

	//cpu needs to be free to dispatch.....
	if(cpuBusy == false && readyq != NULL)
	{
		my_queue* head = popQ(&readyq, &ready_tail);
		dispatch(&head);
	}

}


// This routine adds the specified task to the Processing queue.
// Maintaining FCFS philosophy, it is added to end
void dispatch(my_queue** t) {

	my_queue* task = *t;

	//update Statistics
	updateState(task);
	int waitTime = cpuTimer - task->this_task.arrival_time;
	totalWait+=waitTime;
	processesStarted++;
	if (waitTime > maxWait)
		maxWait = waitTime;

	*t = task;
}



//This routine updates the process states in the PCB (Process Control Block)
//Queues are updated maintaining FCFS philosophy
void updateState(my_queue* task)
{ 
	cout<<cpuTimer<< "||RUNNING: "<<task->this_task.taskid<<" runTime :"<<task->this_task.time_run<<endl;
	task->this_task.time_run++;
	
	if(task->this_task.est_runtime == task->this_task.time_run)						//Terminate the task after estimated time equals the time run
	{	
	cout<<cpuTimer<<"||TERMINATED TASK: "<<task->this_task.taskid<<endl;				
	task->this_task.state= TERMINATED;
	}
	else																		
		{
			if(task->this_task.time_run ==task->this_task.block_startTime )			//Block Task when the time run equals block startTime
			{
			cout<<cpuTimer<<"||BLOCK TASK: "<<task->this_task.taskid<<endl;
			task->this_task.arrival_time=task->this_task.block_waitTime + cpuTimer+1;
			task->this_task.state= BLOCKED;
			cout<<"BLOCK WAIT TIME: "<<task->this_task.block_waitTime<<endl;
			pushQ(&jobq,&task,&job_tail);
		
			}
			else{									
				task->this_task.state= READY;									
				pushQ(&readyq, &task, &ready_tail);					//When the process returns from block state it is addded to the back of the queue
				}
			
		}
}