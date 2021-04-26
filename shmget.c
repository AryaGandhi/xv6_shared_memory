#include "types.h"
#include "fcntl.h"
#include "types.h"
#include "user.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"

int main(){
  int shmid = shmget(IPC_PRIVATE, 10000, IPC_CREAT|0666);
  if(shmid < 0){
    printf(1, "error\n");
    exit();
  }
  else
    printf(1, "%d\n", shmid);
  char * shm = shmat(shmid, (void *)0, 0);
  if(shm < 0)
    printf(1, "error\n");
  else
    printf(1, "%p\n", shm);
  char * s = shm;
  //s[0] = 'a';
  //s[1] = 'b';
  for (char c = 'A'; c <= 'E'; c++)
    *s++ = c;
  *s = 0;
  for (s = shm; *s != 0; s++)
    printf(1, "%c", *s);
  printf(1, "\n");
  int shmid2 = shmget(101, 1000, IPC_CREAT|0666);
  if(shmid2 < 0){
    printf(1, "error\n");
    exit();
  }
  else
    printf(1, "%d\n", shmid2);
  char * shm2 = shmat(shmid2, (void *)0, 0);
  if(shm2 < 0)
    printf(1, "error\n");
  else
    printf(1, "%p\n", shm2);
  char * s2 = shm2;
  for (char c2 = 'A'; c2 <= 'Z'; c2++)
    *s2++ = c2;
  *s2 = 0;
  for (s2 = shm2; *s2 != 0; s2++)
    printf(1, "%c", *s2);
  printf(1, "\n");
  int shmid3 = shmget(1000, 40960, IPC_CREAT|0666);
  if(shmid3 < 0){
    printf(1, "error\n");
    exit();
  }
  else
    printf(1, "%d\n", shmid3);
  char * shm3 = shmat(shmid3, (void *)0, 0);
  if(shm3 < 0)
    printf(1, "error\n");
  else
    printf(1, "%p\n", shm3);
  char * s3 = shm3;
  for (char c3 = 'A'; c3 <= 'M'; c3++)
    *s3++ = c3;
  *s3 = 0;
  for (s3 = shm3; *s3 != 0; s3++)
    printf(1, "%c", *s3);
  printf(1, "\n");
  printf(1, "%p\n", shm3);
  int shmdet = shmdt(shm3);
  printf(1, "%d\n", shmdet);
  if(shmdet == -1){
    printf(1, "error\n");
    exit();
  }
  if(shmdet == 0){
    printf(1, "shmdt successful\n");
  }
  exit();
}
