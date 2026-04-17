
# include <stddef.h>
# include <stdbool.h>

# define NA -1

typedef enum PolicyType policy_t;
typedef struct TaskType task_t;
typedef struct QueueType queue_t;
typedef struct SimulatorType sim_t;

task_t * task_ConstructTask(int tid, int arrival, int service);
void task_DestructTask(task_t * task);
bool task_IsFinsihed(task_t * task);
bool task_HasArrived(task_t * task, size_t currtime);


queue_t * queue_ConstructQueue(void);
void queue_DestructQueue(queue_t * queue);
void queue_InsertFront(queue_t * queue, task_t * task);
void queue_InsertBack(queue_t * queue, task_t * task);
void queue_InsertSJF(queue_t * queue, task_t * tasK);
task_t * queue_RemoveFront(queue_t * queue);
task_t * queue_RemoveBack(queue_t * queue);
bool queue_IsEmpty(queue_t * queue);

sim_t * sim_ConstructSim(const char * readfrom, policy_t policy, unsigned q);
void sim_DestructSim(sim_t * sim);


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

struct SimulatorType
{
  queue_t * inactive;
  queue_t * active;
  queue_t * finsihed;
  
  unsigned quantum; // Time quantum (custom for RR, 1 otherwise).
  size_t counter; // Internal counter to track simulation time
  policy_t policy; 
};







