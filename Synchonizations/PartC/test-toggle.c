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

#define NUM_ITER 10

zem_t a_done;
zem_t b_done;
zem_t c_done;

void *justprint_a(void *data)
{
  int thread_id = *((int *)data);
  while(1){
    zem_down(&c_done);
    for(int i=0; i < NUM_ITER; i++){
      printf("This is thread %d\n", thread_id);
    }
    zem_up(&a_done);
  }
  return 0;
}

void *justprint_b(void *data)
{
  int thread_id = *((int *)data);
  while(1){
    zem_down(&a_done);
    for(int i=0; i < NUM_ITER; i++){
      printf("This is thread %d\n", thread_id);
    }
    zem_up(&b_done);
  }
  return 0;
}

void *justprint_c(void *data)
{
  int thread_id = *((int *)data);
  while(1){
    zem_down(&b_done);
    for(int i=0; i < NUM_ITER; i++){
      printf("This is thread %d\n", thread_id);
    }
    zem_up(&c_done);
  }
  return 0;
}

int main(int argc, char *argv[])
{
  
  zem_init(&a_done, 0);
  zem_init(&b_done, 0);
  zem_init(&c_done, 1);
  
  pthread_t a, b, c;
  int a_id=0, b_id=1, c_id=2;
  
  pthread_create(&a, NULL, justprint_a, &a_id);
  pthread_create(&b, NULL, justprint_b, &b_id);
  pthread_create(&c, NULL, justprint_c, &c_id);
      
  pthread_join(a, NULL);
  pthread_join(b, NULL);
  pthread_join(c, NULL);
  
  
  return 0;
}
