
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <assert.h>
# include "policy.h"

/**
 * Constructor for task_t structure types. 
 */
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

/**
 * Destructor for task_t structure types. 
 */
void task_DestructTask(task_t * task)
{
  assert(task);
  free(task);
}

/**
 * Boolean function which returns true if a 
 * task has finished in the simulation, false otherwise.
 */
bool task_HasFinished(task_t * task)
{
  assert(task);
  if (task->rst <= 0) return true;
  else return false;
}

/**
 * Boolean function which returns true if a task has "arrived"
 * to the ready (or "active") queue in the simulation, false otherwise.
 */
bool task_HasArrived(task_t * task, size_t currtime)
{
  assert(task);
  if (task->arrival <= currtime) return true;
  else return false;
}

/**
 * Constructor for queue_t structure types.
 */
queue_t * queue_ConstructQueue(void)
{
  queue_t * queue = (queue_t *)calloc(1, sizeof(queue_t));
  return queue;
}

/**
 * Destructor for queue_t structure types.
 */
void queue_DestructQueue(queue_t * queue)
{
  assert(queue);
  
  while (!queue_IsEmpty(queue))
    task_DestructTask(queue_RemoveFront(queue));

  free(queue);
}

/**
 * Inserts a task at the front of the queue.
 */
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

/**
 * Inserts a task at the back of the queue.
 */
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

/**
 * Special insertion function for running simulations 
 * where the policy in place is SJF.
 */
void queue_InsertSJF(queue_t * queue, task_t * task)
{
  assert(queue);
  assert(task);

  if (queue_IsEmpty(queue))
    return queue_InsertFront(queue, task);
  
  task_t * rover = queue->front;
  while (rover)
  {
    if (rover->rst > task->rst)
    {
      if (rover == queue->front)
        return queue_InsertFront(queue, task);
      else
      {
        task->next = rover->next;
        if (task->next)
          task->next->prev = task;
        
        task->prev = rover->prev;
        task->prev->next = task;
        rover->prev = task;
      }
    }
    else
    {
      rover = rover->next;
    }
  }
  
  return queue_InsertBack(queue, task);
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

task_t * queue_RemoveIndex(queue_t * queue, size_t index)
{
  assert(queue);
  if ((index < 0) || (index >= queue->length))
    return NULL;
  else if (index == 0)
    return queue_RemoveFront(queue);
  else if (index == queue->length - 1)
    return queue_RemoveBack(queue);
  
  task_t * rover = queue->front;
  for (size_t i = 0; i < index; i++)
    rover = rover->next;
  
  if (rover->next)
    rover->next->prev = rover->prev;
  if (rover->prev)
  rover->prev->next = rover->next;

  rover->next = NULL;
  rover->prev = NULL;

  queue->length--;
  return rover;
}


bool queue_IsEmpty(queue_t * queue)
{
  assert(queue);
  if (queue->length == 0) return true;
  else return false;
}


machine_t * machine_ConstructMachine(const char * readfrom, policy_t policy, unsigned q)
{
  assert(readfrom);

  machine_t * machine = (machine_t *)malloc(sizeof(machine_t));

  machine->policy = policy;
  machine->counter = 0;

  machine->inactive = queue_ConstructQueue();
  machine->active   = queue_ConstructQueue();
  machine->finished = queue_ConstructQueue();
  machine->quantum  = q;

  FILE * fileptr = fopen(readfrom, "r");
  int temp_service, temp_arrival, tid_counter;

  while (fscanf(fileptr, "%d %d", &temp_service, &temp_arrival) == 2)
  {
    tid_counter++;
    if (temp_arrival > 0)
    {
      if (policy == SJF)
        queue_InsertSJF(machine->inactive, task_ConstructTask(tid_counter, temp_arrival, temp_service));
      else
        queue_InsertFront(machine->inactive, task_ConstructTask(tid_counter, temp_arrival, temp_service));
    }
    else
    {
      if (policy == SJF)
        queue_InsertSJF(machine->active, task_ConstructTask(tid_counter, temp_arrival, temp_service));
      else
        queue_InsertFront(machine->active, task_ConstructTask(tid_counter, temp_arrival, temp_service));
    }
  }
  fclose(fileptr); 
  return machine;
}


void machine_DestructMachine(machine_t * machine)
{
  assert(machine);

  queue_DestructQueue(machine->inactive);
  queue_DestructQueue(machine->active);
  queue_DestructQueue(machine->finished);

  free(machine);
}


void machine_CheckForArrivals(machine_t * machine)
{
  assert(machine);

  task_t * temp;
  task_t * front = machine->inactive->front;
  size_t index = 0;

  while (front)
  {
    temp  = front;
    front = front->next;
    
    if (task_HasArrived(temp, machine->counter))
    {
      task_t * removed;
      if (temp == machine->inactive->front)
        removed = queue_RemoveFront(machine->inactive);
      else if (temp == machine->inactive->back)
        removed = queue_RemoveBack(machine->inactive);
      else
      {
        temp->prev->next = temp->next;
        temp->next->prev = temp->prev;
        removed = temp;
      }



      if (machine->policy == SJF)
        queue_InsertSJF(machine->active, removed);
      else
        queue_InsertBack(machine->active, removed);
    }
    index++;
  }
}


void machine_CheckForFinished(machine_t * machine)
{
  assert(machine);

  task_t * temp;
  task_t * front = machine->active->front;
  size_t index = 0;

  while (front)
  {
    temp = front;
    front = front->next;

    if (task_HasFinished(temp))
    {
      task_t * removed;
      if (temp == machine->active->front)
        removed = queue_RemoveFront(machine->active);
      else if (temp == machine->active->back)
        removed = queue_RemoveBack(machine->active);
      else
      {
        temp->prev->next = temp->next;
        temp->next->prev = temp->prev;
        removed = temp;
      }

      // TODO: Write function to calculate statistics for performance metrics.
      queue_InsertBack(machine->finished, removed);
    }

    index++;
  }
}


void machine_AllocateResources(machine_t * machine)
{
  assert(machine);
    
  if (machine->policy == RR)
  {
    task_t * rover = machine->active->front;
    while (rover)
    {
      rover->rst -= machine->quantum;
      if (rover->rst < 0)
        rover->rst = 0;
      rover = rover->next;
    }
  }
  else
  {
    task_t * front = machine->active->front;
    front->rst -= machine->quantum;
    if (front->rst < 0)
      front->rst = 0;
  }
}

void machine_PerformFullCycle(machine_t * machine)
{
  assert(machine);
  machine_CheckForArrivals(machine);
  machine_AllocateResources(machine);
  machine->counter++;
  machine_CheckForFinished(machine);
}