#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>

#define NUM_RIDERS 30
#define N 10


pthread_cond_t opr1, opr2, opr3, rider, ride_end;
pthread_mutex_t m;
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
    pthread_mutex_lock(&m);
    printf("Waiting for the new group of riders...\n");
    while(rider_count < N) pthread_cond_wait(&opr1, &m);                      // Waits if the riders waiting are less than N
  
    open_ride();
    for(int i=0; i<N; i++){
      pthread_cond_signal(&rider);                                            // Wakes up the N riders, so that they can enter now
    }
    rider_count=rider_count-N;                                                // As N as entered, now waiting riders are reduced by N
    while(enter_count < N) pthread_cond_wait(&opr2, &m);                      // Waits until all N riders entered the ride
    enter_count=0;
  
    start_ride();
  
    stop_ride();
    for(int j=0; j<N; j++){
      pthread_cond_signal(&ride_end);                                          // Wakes up the N riders, so that they can exit now
    }
    while(exit_count < N) pthread_cond_wait(&opr3, &m);                        // Waits until all N riders leave the ride
    exit_count=0;

    printf("The riders have exited the ride successfully, the ride will start when new group of riders will come.\n");
    
    pthread_mutex_unlock(&m);
  }
  return 0;
}

void *riders(void *data)
{
  int thread_id = *((int *)data);
  
  pthread_mutex_lock(&m);
  rider_count++;
  if(rider_count%N == 0) pthread_cond_signal(&opr1);                           // In every group of N, Nth guy signal
  pthread_cond_wait(&rider, &m);                                               // All waits, until operator wakes N people
  
  enter_ride(thread_id);
  enter_count++;
  if(enter_count == N) pthread_cond_signal(&opr2);                             // Nth person entering, wakes the operator
  pthread_cond_wait(&ride_end, &m);                                            // All these waits to exit
  
  exit_ride(thread_id);
  exit_count++;
  if(exit_count == N) pthread_cond_signal(&opr3);                              // Nth person leaving, wakes the operator
  pthread_mutex_unlock(&m);
  
  
  return 0;
}

int main(int argc, char *argv[])
{

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
