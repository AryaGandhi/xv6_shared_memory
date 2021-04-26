#define SHMMNI 1024  //System-wide limit on the number of shared memory segments
#define SHMALL 10240  //System-wide limit on the total amount of shared memory, measured in units of page size.
#define SHMMAX 40960  //(40KB)Maximum size in bytes for a shared memory segment.
#define SHMMIN 1  //Minimum size in bytes for a shared memory segment(effectively 4096 B because page size is 4096 B)

#define IPC_CREAT 01000  //Create key if key does not exist.
#define IPC_EXCL 02000  //Fail if key exists.

#define IPC_PRIVATE 0

#define IPC_RMID 0  //Remove identifier.
#define IPC_SET 1  //Set `ipc_perm' options.
#define IPC_STAT 2  //Get `ipc_perm' options.
#define IPC_INFO 3  //See ipcs.


// Per-CPU state
struct cpu {
  uchar apicid;                // Local APIC ID
  struct context *scheduler;   // swtch() here to enter scheduler
  struct taskstate ts;         // Used by x86 to find stack for interrupt
  struct segdesc gdt[NSEGS];   // x86 global descriptor table
  volatile uint started;       // Has the CPU started?
  int ncli;                    // Depth of pushcli nesting.
  int intena;                  // Were interrupts enabled before pushcli?
  struct proc *proc;           // The process running on this cpu or null
};

extern struct cpu cpus[NCPU];
extern int ncpu;

//PAGEBREAK: 17
// Saved registers for kernel context switches.
// Don't need to save all the segment registers (%cs, etc),
// because they are constant across kernel contexts.
// Don't need to save %eax, %ecx, %edx, because the
// x86 convention is that the caller has saved them.
// Contexts are stored at the bottom of the stack they
// describe; the stack pointer is the address of the context.
// The layout of the context matches the layout of the stack in swtch.S
// at the "Switch stacks" comment. Switch doesn't save eip explicitly,
// but it is on the stack and allocproc() manipulates it.
struct context {
  uint edi;
  uint esi;
  uint ebx;
  uint ebp;
  uint eip;
};

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

struct ipc_perm{
  unsigned short mode;  //Permissions + SHM_DEST and SHM_LOCKED flags
};

struct shmid_ds{
  struct ipc_perm shm_perm;  //Ownership and permissions
  int shm_segsz; //Size of segment (bytes)
  int shm_cpid;  //PID of creator
  int shm_lpid;  //PID of last shmat()/shmdt()
  int shm_attaches;  //No. of current attaches
};

int total_shared_memory;  //total amount of shared memory, measured in units of page size(4k), should be less than SHMALL
int no_of_shared_memory_segments;  //total number of shared memory segments.

struct glob_shm{
  int key;
  int shmid;
  void * memory[10];
  struct shmid_ds shmid_ds;
}glob_shm[SHMMNI];

// Track mapped shared pages
struct proc_shm{
  int key;
  void *va;
};

// Per-process state
struct proc {
  uint sz;                     // Size of process memory (bytes)
  pde_t* pgdir;                // Page table
  char *kstack;                // Bottom of kernel stack for this process
  enum procstate state;        // Process state
  int pid;                     // Process ID
  struct proc *parent;         // Parent process
  struct trapframe *tf;        // Trap frame for current syscall
  struct context *context;     // swtch() here to run process
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)
  void * shmsz;                //current size of shared memory
  struct proc_shm proc_shm[16];// Pages shared by process
};

// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap
