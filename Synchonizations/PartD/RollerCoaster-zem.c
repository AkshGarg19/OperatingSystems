#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "zemaphore.h"

#define NUM_RIDERS 30
#define N 10


zem_t opr1, opr2, opr3, rider, ride_end, mutex;
int rider_count=0;
int enter_count=0;
int exit_count=0;

void enter_ride(int id){
  printf("The rider with ticket number %d has entered the ride.\n", id);
  usleep(200000);
}

void exit_ride(int id){
  printf("The rider with ticket number %d has exited the ride.\n", id);
  usleep(200000);
}

void open_ride(void){
  printf("The riders group have came, opening the ride for them.\n");
  usleep(500000);
}

void start_ride(void){
  printf("The riders group have entered the ride, starting the ride for them.\n");
  sleep(5);
}

void stop_ride(void){
  printf("The ride has been ended!\n");
  usleep(500000);
}

void *operator(void *data)
{
  while(1){
    printf("Waiting for the new group of riders...\n");
    zem_down(&opr1);                                                    // Waits if the riders waiting are less than N
  
    open_ride();
    for(int i=0; i<N; i++){
      zem_up(&rider);                                                   // Wakes up the N riders, so that they can enter now
    }
    zem_down(&opr2);                                                    // Waits until all N riders entered the ride
  
    start_ride();
  
    stop_ride();
    for(int j=0; j<N; j++){
      zem_up(&ride_end);                                                // Wakes up the N riders, so that they can exit now
    }
    zem_down(&opr3);                                                    // Waits until all N riders leave the ride

    printf("The riders have exited the ride successfully, the ride will start when new group of riders will come.\n");
  }
  return 0;
}

void *riders(void *data)
{
  int thread_id = *((int *)data);
  
  zem_down(&mutex);
  rider_count++;
  zem_up(&mutex);
  
  if(rider_count%N == 0) zem_up(&opr1);                                 // In every group of N, Nth guy signal that one group is ready
  zem_down(&rider);                                                     // All waits, until operator wakes N people
  
  zem_down(&mutex);
  enter_ride(thread_id);
  zem_up(&mutex);
  
  zem_down(&mutex);
  enter_count++;
  zem_up(&mutex);
  
  if(enter_count%N == 0) zem_up(&opr2);                                 // In every group, Nth person entering, wakes the operator
  zem_down(&ride_end);                                                  // All these waits to exit
  
  zem_down(&mutex);
  exit_ride(thread_id);
  zem_up(&mutex);
  
  zem_down(&mutex);
  exit_count++;
  zem_up(&mutex);
  
  if(exit_count%N == 0) zem_up(&opr3);                                  // In evry group, Nth person leaving, wakes the operator
  
  return 0;
}

int main(int argc, char *argv[])
{
  zem_init(&opr1, 0);
  zem_init(&opr2, 0);
  zem_init(&opr3, 0);
  zem_init(&rider, 0);
  zem_init(&ride_end, 0);
  zem_init(&mutex, 1);
  
  pthread_t operate;
  int opid = 0;
  
  pthread_create(&operate, NULL, operator, &opid);

  pthread_t riderss[NUM_RIDERS];
  int rider_id[NUM_RIDERS];

  
  for(int i =0; i < NUM_RIDERS; i++){
    rider_id[i] = i+1;
    pthread_create(&riderss[i], NULL, riders, (void *)&rider_id[i]);
  }
      
  for(int i =0; i < NUM_RIDERS; i++){
    pthread_join(riderss[i], NULL);
  }
  
  pthread_join(operate, NULL);
    
  
  return 0;
}
