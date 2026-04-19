
# ifndef __POLICY_H__
# define __POLICY_H__

# include <stddef.h>
# include <stdbool.h>

# define NA -1

typedef enum PolicyType policy_t;
typedef struct TaskType task_t;
typedef struct QueueType queue_t;
typedef struct MachineType machine_t;
typedef struct ClusterType cluster_t;

task_t * task_ConstructTask(int tid, int arrival, int service);
void task_DestructTask(task_t * task);
bool task_HasFinished(task_t * task);
bool task_HasArrived(task_t * task, size_t currtime);
void task_PrintStatistics(task_t * task);


queue_t * queue_ConstructQueue(void);
void queue_DestructQueue(queue_t * queue);
void queue_InsertFront(queue_t * queue, task_t * task);
void queue_InsertBack(queue_t * queue, task_t * task);
void queue_InsertSJF(queue_t * queue, task_t * tasK);
task_t * queue_RemoveFront(queue_t * queue);
task_t * queue_RemoveBack(queue_t * queue);
task_t * queue_RemoveIndex(queue_t * queue, size_t index);
bool queue_IsEmpty(queue_t * queue);

machine_t * machine_ConstructMachine(const char * readfrom, policy_t policy, unsigned q);
void machine_DestructMachine(machine_t * machine);
void machine_CheckForArrivals(machine_t * machine);
void machine_CheckForFinished(machine_t * machine);
void machine_AllocateResources(machine_t * machine);
void machine_PerformFullCycle(machine_t * machine);
void machine_PrintSummaryStatistics(machine_t * machine);

enum PolicyType
{
  FIFO = 0,
  SJF  = 1,
  RR   = 2,
};

struct TaskType
{
  task_t * next;
  task_t * prev;

  int tid; // task id
  int service; // service time
  int rst; // remaining service time
  int arrival; // arrival time
  int wait; // wait time
  int response; // response time
  int completion; // completion time
};

struct QueueType
{
  task_t * front;
  task_t * back;
  size_t length;
};

struct MachineType
{
  queue_t * inactive;
  queue_t * active;
  queue_t * finished;
  
  unsigned quantum; // Time quantum (custom for RR, 1 otherwise).
  size_t counter;   // Internal counter to track simulation time
  policy_t policy;  // Scheduling policy of the machine
};

/**
 * Data structure which provides abstraction for a computing cluster,
 * allowing for multiple machines with their own scheduling policies to be 
 * simulated as if they were running in parallel/concurrently.
 */
struct ClusterType
{
  machine_t ** machines;
  size_t occupied;
  size_t capacity;
};


# endif
