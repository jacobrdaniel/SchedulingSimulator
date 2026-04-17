
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <assert.h>
# include "policy.h"


task_t * task_ConstructTask(int tid, int arrival, int service)
{
  task_t * task = (task_t *)malloc(sizeof(task_t));

  task->tid = tid;
  task->service = service;
  task->rst = service;
  task->arrival = arrival;
  task->wait = NA;
  task->response = NA;
  task->completion = NA;

  task->next = NULL;
  task->prev = NULL;

  return task;
}

void task_DestructTask(task_t * task)
{
  assert(task);
  free(task);
}

bool task_IsFinsihed(task_t * task)
{
  assert(task);
  if (task->rst <= 0) return true;
  else return false;
}

bool task_HasArrived(task_t * task, size_t currtime)
{
  assert(task);
  if (task->arrival >= currtime) return true;
  else return false;
}

queue_t * queue_ConstructQueue(void)
{
  queue_t * queue = (queue_t *)calloc(1, sizeof(queue_t));
  return queue;
}

void queue_DestructQueue(queue_t * queue)
{
  assert(queue);
  
  while (!queue_IsEmpty(queue))
    task_DestructTask(queue_RemoveFront(queue));

  free(queue);
}

void queue_InsertFront(queue_t * queue, task_t * task)
{
  assert(queue);
  assert(task);

  if (queue_IsEmpty(queue))
  {
    queue->front = task;
    queue->back  = task;
  }
  else
  {
    task->next = queue->front;
    queue->front->prev = task;
    queue->front = task;
  }

  queue->length++;
}

void queue_InsertBack(queue_t * queue, task_t * task)
{
  assert(queue);
  assert(task);

  if (queue_IsEmpty(queue))
  {
    queue->front = task;
    queue->back  = task;
  }
  else
  {
    task->prev = queue->back;
    queue->back->next = task;
    queue->back = task;
  }

  queue->length++;
}

void queue_InsertSJF(queue_t * queue, task_t * task)
{
  assert(queue);
  assert(task);

  if (queue_IsEmpty(queue))
    return queue_InsertFront(queue, task);
  
  task_t * rover = queue->front;
  while (rover->rst < task->rst)
    rover = rover->next;
  
  if (rover == queue->front)
    return queue_InsertFront(queue, task);
  else if (rover == queue->back)
    return queue_InsertBack(queue, task);
  
  // Insert task before rover

  task->next = rover;
  task->prev = rover->prev;
  task->prev->next = task;
  rover->prev = task;

  queue->length++;
}

task_t * queue_RemoveFront(queue_t * queue)
{
  assert(queue);

  if (queue_IsEmpty(queue))
    return NULL;

  task_t * front = queue->front;
  
  if (!front->next)
  {
    queue->front = NULL;
    queue->back  = NULL;
  }
  else
  {
    queue->front = queue->front->next;
    queue->front->prev = NULL;
  }
    

  queue->length--;
  return front;
}

task_t * queue_RemoveBack(queue_t * queue)
{
  assert(queue);

  if (queue_IsEmpty(queue))
    return NULL;
  
  task_t * back = queue->back;

  if (!back->prev)
  {
    queue->front = NULL;
    queue->back  = NULL;
  }
  else
  {
    queue->back = queue->back->prev;
    queue->back->next = NULL;
  }

  queue->length--;
  return back;
}

bool queue_IsEmpty(queue_t * queue)
{
  assert(queue);
  if (queue->length == 0) return true;
  else return false;
}

sim_t * sim_ConstructSim(const char * readfrom, policy_t policy, unsigned q)
{
  assert(readfrom);

  sim_t * sim = (sim_t *)malloc(sizeof(sim_t));

  sim->policy = policy;
  sim->counter = 0;

  sim->inactive = queue_ConstructQueue();
  sim->active   = queue_ConstructQueue();
  sim->finsihed = queue_ConstructQueue();
  sim->quantum  = q;

  FILE * fileptr = fopen(readfrom, "r");
  int temp_service, temp_arrival, tid_counter;

  while (fscanf(fileptr, "%d %d", &temp_service, &temp_arrival))
  {
    tid_counter++;
    if (temp_arrival > 0)
    {
      if (policy == SJF)
        queue_InsertSJF(sim->inactive, task_ConstructTask(tid_counter, temp_arrival, temp_service));
      else
        queue_InsertBack(sim->inactive, task_ConstructTask(tid_counter, temp_arrival, temp_service));
    }
    else
    {
      if (policy == SJF)
        queue_InsertSJF(sim->active, task_ConstructTask(tid_counter, temp_arrival, temp_service));
      else
        queue_InsertBack(sim->active, task_ConstructTask(tid_counter, temp_arrival, temp_service));
    }
  }
  fclose(fileptr); 
  return sim;
}


void sim_DestructSim(sim_t * sim)
{
  assert(sim);

  queue_DestructQueue(sim->inactive);
  queue_DestructQueue(sim->active);
  queue_DestructQueue(sim->finsihed);

  free(sim);
}
