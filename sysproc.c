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
    if(glob_shm[i].key == key && glob_shm[i].shmid_ds.shm_segsz >= size && (flag == (IPC_CREAT|IPC_EXCL|0666) || flag == (IPC_CREAT|IPC_EXCL|0444)))
      return -1;  //EEXIST
    if(glob_shm[i].key == key && glob_shm[i].shmid_ds.shm_segsz < size)  //EINVAL
      return -1;
    if(glob_shm[i].key == key && glob_shm[i].shmid_ds.shm_segsz >= size)
      return glob_shm[i].shmid;
  } 
  if(key == IPC_PRIVATE){  //assigning a random key if value of key is given IPC_PRIVATE
    if(glob_shm[0].key == -1)
      key = 7845152;
    else{
      key = 142589 * glob_shm[0].key;
      for(int i = 0; i < SHMMNI; i++){
        if(glob_shm[i].key == key)
          key = glob_shm[i].key / 3;
        if(glob_shm[i].key == key)
          break;
      }
    }
  }
  if(!(flag == (IPC_CREAT|IPC_EXCL|0666) || flag == (IPC_CREAT|IPC_EXCL|0444) || flag == (IPC_CREAT|0666) || flag == (IPC_CREAT|0444)))
    return -1;  //ENOENT
  if(((PGROUNDUP(size))/PGSIZE + total_shared_memory) > SHMALL || no_of_shared_memory_segments >= SHMMNI) //ENOSPC
    return -1;
  if(PGROUNDUP(size) > SHMMAX)  //EINVAL
    return -1;
  struct proc *curproc = myproc();
  int flag2 = -1;
  for(int i = 0; i < SHMMNI; i++){
    if(glob_shm[i].key != -1)
      continue;
    if(glob_shm[i].key == -1){
      flag2 = i;
      break;
    }
  }
  if(flag2 == -1)  //limit for number of shared memory segments for system reached
    return -1; 
  int check = shmgetuvm(size, flag2);
  if(check == 0)
    return -1;
  glob_shm[flag2].key = key;
  glob_shm[flag2].shmid_ds.shm_segsz = size;
  glob_shm[flag2].shmid_ds.shm_cpid = curproc->pid;
  glob_shm[flag2].shmid_ds.shm_lpid = 0;
  glob_shm[flag2].shmid_ds.shm_nattch = 0;
  int flagend = flag & 0x000001FF;
  int flagoctal = 0;
  int i = 1;
  while(flagend != 0){  /* decimal to octal conversion code via https://www.programiz.com/c-programming/examples/octal-decimal-convert */
    flagoctal += (flagend % 8) * i;
    flagend /= 8;
    i *= 10;
  }
  glob_shm[flag2].shmid_ds.shm_perm.mode = flagoctal;  //to get the least significant 9 bits of the flag
  glob_shm[flag2].shmid_ds.shm_perm.key = key;
  total_shared_memory = total_shared_memory + (PGROUNDUP(size))/PGSIZE;
  no_of_shared_memory_segments = no_of_shared_memory_segments + 1;
  return glob_shm[flag2].shmid;
}

void * sys_shmat(void){
  int shmid;
  char * shmaddr;
  int flag;
  if(argint(0, &shmid) < 0)
    return (void *)-1;
  if(argptr(1, &shmaddr, sizeof(*shmaddr)) < 0)
    return (void *)-1;
  if(argint(2, &flag) < 0)
    return (void *)-1;
  if(shmid > SHMMNI || shmid < 0)  //EINVAL
    return (void *)-1;
  if(glob_shm[shmid].key == -1)  //EINVAL
    return (void *)-1;
  int permissions = glob_shm[shmid].shmid_ds.shm_perm.mode;
  if(flag == SHM_RDONLY)
    permissions = 444;
  struct proc *curproc = myproc();
  int flag1 = -1;
  for(int i = 0; i < 16; i++){
    if(curproc->proc_shm[i].shmid != -1)
      continue;
    if(curproc->proc_shm[i].shmid == -1){
      flag1 = i;
      break;
    }
  }
  if(glob_shm[shmid].shmid_ds.shm_perm.mode == 444 && flag != SHM_RDONLY)  //EACCESS
    return (void *)-1;
  if(flag1 == -1)  //limit for number of shared memory segments for that process reached
    return (void *)-1;
  void * currentsize = curproc->shmsz;
  int pages = PGROUNDUP(glob_shm[shmid].shmid_ds.shm_segsz) / PGSIZE;
  void * check = shmmapmem(curproc->pgdir, currentsize, pages, shmid, permissions);
  if(!check)  //ENOMEM
    return (void *)-1;
  curproc->proc_shm[flag1].shmid = glob_shm[shmid].shmid;
  curproc->proc_shm[flag1].va = check;
  glob_shm[shmid].shmid_ds.shm_lpid = curproc->pid;
  glob_shm[shmid].shmid_ds.shm_nattch++;
  curproc->shmsz = (void *)((int)curproc->shmsz + PGROUNDUP(glob_shm[shmid].shmid_ds.shm_segsz));
  return check;
}

int sys_shmdt(void){
  int shmaddress;
  if(argint(0, &shmaddress) < 0)
    return -1;
  void * shmaddr = (void *)shmaddress;
  struct proc *curproc = myproc();
  int flag1 = -1;
  for(int i = 0; i < 16; i++){
    if(curproc->proc_shm[i].va != shmaddr)
      continue;
    if(curproc->proc_shm[i].va == shmaddr){
      flag1 = i;
      break;
    }
  }
  if(flag1 == -1)  //EINVAL
    return -1;
  int shmid = curproc->proc_shm[flag1].shmid;
  if(glob_shm[shmid].key == -1)
    return -1;
  int oldsz = (int)curproc->shmsz;
  int newsz = (int)curproc->shmsz - glob_shm[shmid].shmid_ds.shm_segsz;
  curproc->shmsz = (void *)deallocshm(curproc->pgdir, oldsz, newsz);
  curproc->proc_shm[shmid].va = 0;
  curproc->proc_shm[shmid].shmid = -1;
  glob_shm[shmid].shmid_ds.shm_nattch--;
  if(glob_shm[shmid].shmid_ds.shm_perm.rem == 1 && glob_shm[shmid].shmid_ds.shm_nattch == 0){  //if nattch becomes zero and segment is marked for deletion, it is deleted
    for(int i = 0; i < 10; i++){
      if(glob_shm[shmid].memory[i]){
        kfree((char *)P2V(glob_shm[shmid].memory[i]));
      }
    }
    for(int j = 0; j < 10; j++)
      glob_shm[shmid].memory[j] = (void *)0;
    glob_shm[shmid].key = -1;
    glob_shm[shmid].shmid_ds.shm_perm.key = -1;
    glob_shm[shmid].shmid_ds.shm_perm.mode = -1;
    glob_shm[shmid].shmid_ds.shm_perm.rem = 0;
    glob_shm[shmid].shmid_ds.shm_segsz = -1;
    glob_shm[shmid].shmid_ds.shm_cpid = -1;
    glob_shm[shmid].shmid_ds.shm_lpid = -1;
    glob_shm[shmid].shmid_ds.shm_nattch = 0;
  }
  glob_shm[shmid].shmid_ds.shm_lpid = curproc->pid;
  return 0;
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
  if(shmid > SHMMNI)
    return -1;  //EINVAL
  if(glob_shm[shmid].key == -1)  //EINVAL
    return -1;
  if(!(cmd == IPC_STAT || cmd == IPC_SET || cmd == IPC_INFO || cmd == IPC_RMID))  //EINVAL
    return -1;
  if(cmd == IPC_STAT){
    if(!(glob_shm[shmid].shmid_ds.shm_perm.mode == 444 || glob_shm[shmid].shmid_ds.shm_perm.mode == 666))  //EACCES
      return -1;
    * buf = glob_shm[shmid].shmid_ds;
    return 0;
  }
  else if(cmd == IPC_SET){
    if(glob_shm[shmid].shmid_ds.shm_segsz != buf->shm_segsz)  //EFAULT
      return -1;
    glob_shm[shmid].shmid_ds = *buf;
    if(glob_shm[shmid].shmid_ds.shm_perm.mode == 444 || glob_shm[shmid].shmid_ds.shm_perm.mode == 666)
      return 0;
    else{
      int flag = glob_shm[shmid].shmid_ds.shm_perm.mode;
      int flagend = flag & 0x000001FF;
      int flagoctal = 0;
      int i = 1;
      while(flagend != 0){
        flagoctal += (flagend % 8) * i;
        flagend /= 8;
        i *= 10;
      }
      glob_shm[shmid].shmid_ds.shm_perm.mode = flagoctal;
    }
    return 0;
  }
  else if(cmd == IPC_INFO){
    * buf = glob_shm[shmid].shmid_ds;
    for(int i = 0; i < SHMMNI; i++){
      if(glob_shm[i].key != -1)
        continue;
      else
        return i;
    }
    return -1;
  }
  else if(cmd == IPC_RMID){
    if(glob_shm[shmid].shmid_ds.shm_nattch == 0){
      for(int i = 0; i < 10; i++){
        if(glob_shm[shmid].memory[i])
          kfree((char *)P2V(glob_shm[shmid].memory[i]));
      }
      for(int j = 0; j < 10; j++)
        glob_shm[shmid].memory[j] = (void *)0;
      glob_shm[shmid].key = -1;
      glob_shm[shmid].shmid_ds.shm_perm.key = -1;
      glob_shm[shmid].shmid_ds.shm_perm.mode = -1;
      glob_shm[shmid].shmid_ds.shm_perm.rem = 0;
      glob_shm[shmid].shmid_ds.shm_segsz = -1;
      glob_shm[shmid].shmid_ds.shm_cpid = -1;
      glob_shm[shmid].shmid_ds.shm_lpid = -1;
      glob_shm[shmid].shmid_ds.shm_nattch = 0;
    }
    else if(glob_shm[shmid].shmid_ds.shm_nattch != 0){
      glob_shm[shmid].shmid_ds.shm_perm.rem = 1;
    }
    return 0;
  }
  return -1;
}

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
