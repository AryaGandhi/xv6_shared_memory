#include "types.h"
#include "fcntl.h"
#include "types.h"
#include "user.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"

int main(){
  int shmid = shmget(100, 10, IPC_CREAT|IPC_EXCL|0666);
  if(shmid < 0)
    printf(1, "error\n");
  else
    //printf(1, "succesful\n");
    printf(1, "%d\n", shmid);
  int shmid2 = shmget(10, 10, IPC_CREAT|IPC_EXCL|0666);
  if(shmid2 < 0)
    printf(1, "error\n");
  else
    //printf(1, "succesful\n");
    printf(1, "%d\n", shmid2);
  int shmid3 = shmget(100, 10, IPC_CREAT|0666);
  if(shmid3 < 0)
    printf(1, "error\n");
  else
    //printf(1, "succesful\n");
    printf(1, "%d\n", shmid3);
  exit();
}
