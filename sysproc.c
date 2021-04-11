#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int sys_shmget(void){
  int key;
  int size;
  int flag;
  if(argint(0, &key) < 0)
    return -1;
  if(argint(1, &size) < 0)
    return -1;
  if(argint(2, &flag) < 0)
    return -1;
  for(int i = 0; i < SHMMNI; i++){  //if shared memory already exists for that key
    if(glob_shm[i].key == key && glob_shm[i].shmid_ds.shm_segsz >= size)
      return glob_shm[i].shmid;
    if(glob_shm[i].key == key && glob_shm[i].shmid_ds.shm_segsz < size)  //EINVAL
      return -1;
  }
  if((PGROUNDUP(size) + total_shared_memory)/PGSIZE > SHMALL || no_of_shared_memory_segments >= SHMMNI) //ENOSPC
    return -1;
  if(PGROUNDUP(size) > SHMMAX)  //EINVAL 
    return -1;
  struct proc *curproc = myproc();
  int currentsize = curproc->shmsz;
  int flag1 = -1;
  for(int i = 0; i < 16; i++){
    if(curproc->proc_shm[i].key != -1)
      continue;
    if(curproc->proc_shm[i].key == -1){
      flag1 = i;
      break;
    }
  }
  int flag2 = -1;
  for(int i = 0; i < SHMMNI; i++){
    if(glob_shm[i].key != -1)
      continue;
    if(glob_shm[i].key == -1){
      flag2 = i;
      break;
    }
  }
  char * address;
  if(flag1 == -1)  //limit for number of shared memory segments for that process reached
    return -1;
  if(flag2 == -1)  //limit for number of shared memory segments for system reached
    return -1;
  address = shmgetuvm(curproc->pgdir, currentsize, currentsize + size);
  if(address == 0)
    return -1;
  glob_shm[flag2].key = key;
  glob_shm[flag2].addr = address;
  glob_shm[flag2].shmid_ds.shm_segsz = size;
  glob_shm[flag2].shmid_ds.shm_cpid = curproc->pid;
  glob_shm[flag2].shmid_ds.shm_lpid = 0;
  glob_shm[flag2].shmid_ds.shm_attaches = 0;
  return glob_shm[flag2].shmid;
}

/*
int sys_shmat(void){
  int smhid;
  void * shmaddr;
  int flag;
  if(argint(0, &shmid) < 0)
    return -1;
  if(argptr(1, &shmaddr, sizeof(*shmaddr)) < 0)
    return -1;
  if(argint(2, &flag) < 0)
    return -1;
}

int sys_shmdt(void){
  void * shmaddr;
  if(argptr(0, &shmaddr, sizeof(*shmaddr)) < 0)
    return -1;
}

int sys_shmctl(void){
  int shmid;
  int cmd;
  struct shmid_ds * buf;
  if(argint(0, &shmid) < 0)
    return -1;
  if(argint(1, &cmd) < 0)
    return -1;
  if(argptr(2, (void*)&buf, sizeof(*buf)) < 0)
    return -1;
}*/

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
