#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "zemaphore.h"


pthread_mutex_t m;
pthread_cond_t cond1;

void zem_init(zem_t *s, int value) {
  s->value = value;
  return;
}

void zem_down(zem_t *s) {
  pthread_mutex_lock(&m);
  while(s->value <= 0){
    pthread_cond_wait(&cond1, &m);
  };
  s->value--;
  pthread_mutex_unlock(&m);
  return;
}

void zem_up(zem_t *s) {
  pthread_mutex_lock(&m);
  s->value++;
  pthread_cond_broadcast(&cond1);
  pthread_mutex_unlock(&m);
  return;
}
