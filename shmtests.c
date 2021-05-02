#include "types.h"
#include "fcntl.h"
#include "types.h"
#include "user.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"

int main(){
  //shmget()
  printf(1, "\nSHMGET\n");
  int shmid = shmget(10, 10000, IPC_CREAT|IPC_EXCL|0666);
  if(shmid < 0)
    printf(1, "error in shmget\n");
  else
    printf(1, "Memory segment of size 10000 and flags IPC_CREAT|IPC_EXCL|0666 created\n");
  int shmid2 = shmget(11, 1000, IPC_CREAT|IPC_EXCL|0444);
  if(shmid2 < 0)
    printf(1, "error in shmget\n");
  else
    printf(1, "Memory segment of size 1000 and flags IPC_CREAT|IPC_EXCL|0444 created\n");
  int shmid3 = shmget(12, 4096, IPC_CREAT|0444);
  if(shmid3 < 0)
    printf(1, "error in shmget\n");
  else
    printf(1, "Memory segment of size 4096 and flags IPC_CREAT|0444 created\n");
  int shmid6 = shmget(IPC_PRIVATE, 20, IPC_CREAT|IPC_EXCL|0666);
  if(shmid6 < 0)
    printf(1, "error in shmget\n");
  else
    printf(1, "Memory segment of size 20 with key IPC_PRIVATE and flags IPC_CREAT|IPC_EXCL|0666 created\n");
  int shmid9 = shmget(10, 1000, IPC_CREAT|0666);
  if(shmid9 < 0)
    printf(1, "error in shmget\n");
  else
    printf(1, "Memory segment of size 1000 and flags IPC_CREAT|0666 with previously existing key, returned shared memory segment identifier\n");
  int shmid10 = shmget(10, 10001, IPC_CREAT|0444);
  if(shmid10 < 0)
    printf(1, "EINVAL error in shmget\n");
  else
    printf(1, "Memory segment of size 10001(greater than previously set) and flags IPC_CREAT|0444 with previously existing key, returned shared memory segment identifier\n");
  int shmid4 = shmget(18, 40961, IPC_CREAT|0444);
  if(shmid4 < 0)
    printf(1, "EINVAL error in shmget\n");
  else
    printf(1, "Memory segment of size 40961(more than the limit) and flags IPC_CREAT|0444 created\n");
  int shmid5 = shmget(20, 4096, 0666);
  if(shmid5 < 0)
    printf(1, "ENOENT error in shmget\n");
  else
    printf(1, "Memory segment of size 4096 and flags 0666(IPC_CREAT not specified) created\n");
  int shmid7 = shmget(10, 1000, IPC_CREAT|IPC_EXCL|0666);
  if(shmid7 < 0)
    printf(1, "EEXIST error in shmget\n");
  else
    printf(1, "Memory segment of size 1000 and flags IPC_CREAT|IPC_EXCL|0666 with previously existing key, returned shared memory segment identifier\n");
  //shmat()
  printf(1, "\nSHMAT\n");
  void * shm = shmat(shmid, (void *)0, 0);
  if((int)shm < 0)
    printf(1, "error in shmat\n");
  else{
    printf(1, "Shared memory segment attached to address space of process\n");
    char * s = shm;
    printf(1, "Writing A-M to shared memory segment...\n");
    for (char c = 'A'; c <= 'M'; c++)
      *s++ = c;
    *s = 0;
    printf(1, "Reading from shared memory segment in same process...\n");
    for (s = shm; *s != 0; s++)
      printf(1, "%c", *s);
    printf(1, "\n");
  }
  void * shm3 = shmat(shmid3, (void *)0, SHM_RDONLY);
  if((int)shm3 < 0)
    printf(1, "error in shmat\n");
  else
    printf(1, "Shared memory segment attached to address space of process using SHM_RDONLY\n");
  void * shm2000 = shmat(2000, (void *)0, 0);
  if((int)shm2000 < 0)
    printf(1, "EINVAL error in shmat\n");
  else
    printf(1, "Shared memory segment attached to address space of process\n");
  void * shm1000 = shmat(1000, (void *)0, 0);
  if((int)shm1000 < 0)
    printf(1, "EINVAL error in shmat\n");
  else
    printf(1, "Shared memory segment attached to address space of process\n");
  void * shm3_2 = shmat(shmid3, (void *)0, 0);
  if((int)shm3_2 < 0)
    printf(1, "EACCESS error in shmat\n");
  else
    printf(1, "Shared memory segment attached to address space of process\n");
  //shmctl()
  printf(1, "\nSHMCTL\n");
  struct shmid_ds shmid_ds, *buf;
  buf = &shmid_ds;
  int ctl = shmctl(shmid3, IPC_STAT, buf);
  if(ctl < 0)
    printf(1, "error in shmctl with IPC_STAT\n");
  else{
    printf(1, "Values retrieved using IPC_STAT:\n");
    printf(1, "   Permissions = 0%d\n", buf->shm_perm.mode);
    printf(1, "   Key = %d\n", buf->shm_perm.key);
    printf(1, "   Segment size = %d\n", buf->shm_segsz);
    printf(1, "   PID of creator = %d\n", buf->shm_cpid);
    printf(1, "   PID of last shmat()/shmdt() = %d\n", buf->shm_lpid);
    printf(1, "   Number of current attaches = %d\n", buf->shm_nattch);
  }
  buf->shm_perm.mode = 0666;
  int ctl2 = shmctl(shmid3, IPC_SET, buf);
  if(ctl2 < 0)
    printf(1, "error in shmctl with IPC_SET\n");
  else{
    printf(1, "IPC_SET successful\n");
    int ctl3 = shmctl(shmid3, IPC_STAT, buf);
    if(ctl3 < 0)
      printf(1, "error in shmctl with IPC_STAT\n");
    else
      printf(1, "   Updated permissions = 0%d\n", buf->shm_perm.mode);
  }
  struct shmid_ds shmid_ds2, *buf2;
  buf2 = &shmid_ds2;
  int ctl4 = shmctl(shmid, IPC_INFO, buf2);
  if(ctl4 < 0)
    printf(1, "error in shmctl with IPC_INFO\n");
  else{
    printf(1, "SHMMAX %d\n", buf2->shminfo.shmmax);
    printf(1, "SHMMIN %d\n", buf2->shminfo.shmmin);
    printf(1, "SHMMNI %d\n", buf2->shminfo.shmmni);
    printf(1, "SHMSEG %d\n", buf2->shminfo.shmseg);
    printf(1, "SHMALL %d\n", buf2->shminfo.shmall);
  }
  int ctl5 = shmctl(shmid3, IPC_RMID, buf2);
  if(ctl5 < 0)
    printf(1, "error in shmctl with IPC_RMID\n");
  else
    printf(1, "IPC_RMID successful\n");
  struct shmid_ds shmid_ds3, *buf3;
  buf3 = &shmid_ds3;
  int ctl6 = shmctl(2000, IPC_STAT, buf3);
  if(ctl6 < 0)
    printf(1, "EINVAL error in shmctl with IPC_STAT\n");
  else
    printf(1, "shmctl successful with IPC_STAT and invalid shmid\n");
  int ctl7 = shmctl(shmid, IPC_SET, buf3);
  if(ctl7 < 0)
    printf(1, "EFAULT error in shmctl with IPC_SET\n");
  else
    printf(1, "shmctl successful with IPC_SET and invalid buffer\n");
  int ctl8 = shmctl(shmid, 10, buf3);
  if(ctl8 < 0)
    printf(1, "EINVAL error in shmctl due to invalid command\n");
  else
    printf(1, "shmctl successful with invalid command\n");
  //shmdt()
  printf(1, "\nSHMDT\n");
  int shmdet = shmdt(shm3);
  if(shmdet == -1)
    printf(1, "error in shmdt\n");
  if(shmdet == 0)
    printf(1, "shmdt successful\n");
  int shmdet2 = shmdt((void *)-1);
  if(shmdet2 == -1)
    printf(1, "EINVAL error in shmdt\n");
  if(shmdet2 == 0)
    printf(1, "shmdt successful with invalid shmaddr\n");
  //for child process(fork)
  printf(1, "\nShared memory behaviour on fork\n");
  char * s10 = shm;
  printf(1, "current data in the shared memory segment:\n");
  for(s10 = shm; *s10 != 0; s10++)
    printf(1, "%c", *s10);
  printf(1, "\n");
  int pid = fork();
  if(pid < 0)
    printf(1, "error\n");
  if(pid == 0){
    printf(1, "appending to the current data in the shared memory segment while in child...\n");
    for (char c = 'A'; c <= 'E'; c++)
      *s10++ = c;
    *s10 = 0;
  }
  if(pid > 0){
    wait();
    printf(1, "reading the data after child process has written into it\n");
    for(s10 = shm; *s10 != 0; s10++)
      printf(1, "%c", *s10);
    printf(1, "\n");
  }
  exit();
}
