#include "util.h"
#define NPROC 9                // number of PROCs
#define SSIZE 1024             // per proc stack area 
#define RED 
#define NULL 0
typedef enum {FREE, READY, SLEEP, BLOCK, ZOMBIE} STATUS;

typedef struct proc{
  struct proc *next;
  int    ksp;               // saved ksp when not running
  int    status;
  int    priority;
  int    pid;
  int    ppid;                // add pid for identify the proc
  struct proc   *parent;
  int    kstack[SSIZE];      // proc stack area
}PROC;

int  procSize = sizeof(PROC);

PROC proc[NPROC], *running, *freeList, *readyQueue;    // define NPROC procs
extern int color;

enqueue(); 
PROC *dequeue (PROC **queue);
body();
ksleep(int event);
kwakeup(int event);
kexit(int exitValue);
kwait(int *status); 
do_tswitch();
do_kfork();
do_exit();
do_sleep();
do_wakeup();
do_wait();

do_tswitch(){
  tswitch();
}

do_kfork(){
  child = kfork();
  if(child)
    myprintf("Successfully forked %d\n", child->pid);
  else
    myprintf("Failed fork\n");
}

do_exit(){

  myprintf("Please enter an event value: \n");
  getint(stdin);
  running->status = ZOMBIE;
  tswitch();
  exit();
}
do_sleep(){

}
do_wakeup(){

}
do_wait(){

}

ksleep(int event){

}
kwakeup(int event){
}
kexit(int exitValue){
}
kwait(int *status){
}


int scheduler()
{
  printQueue(readyQueue, "readyQueue");
  //  printf("running status %d", running->status);
  if (running->status == READY){
    enqueue(&readyQueue, running);
    //   myprintf("running->pid: %d enqueued\n", running->pid);
  }
  else{
    //    running = dequeue(&readyQueue);
    put_proc(&readyQueue, running);
  }
  //  printQueue(readyQueue, "readyQueue");
  running = dequeue(&readyQueue);
  //  printQueue(readyQueue, "readyQueue");
}

// e.g. get_proc(&freeList);
PROC *get_proc(PROC **list){
  if(*list)
    return dequeue(&(*list));
  return 0;
}
// e.g. put_proc(&freeList, p);
int put_proc(PROC **list, PROC *p) {
  PROC *ptr;
  ptr = *list;

  p->status = FREE;
  if(*list){
    p->next = *list;
    *list = p;
  }
  else {
    *list = p;
    p->next = NULL;
  }
  return 0;
}

// linear queue with running proc in the highest priority 
int enqueue(PROC **queue, PROC *p) {
  PROC *prev, *cur;

  //myprintf("enqueue() priority = %d\n", p->priority);

  prev = *queue;
  // myprintf("prev = %x\n", prev);

  // readyQueue is empty
  if(!prev){
    *queue = p;
    p->next = NULL;
    return 1;
  }
  // readyQueue is NOT empty
  else{
    while(prev->priority >= p->priority && prev->next) {
      prev = prev->next;
    }
    p->next = prev->next;
    prev->next = p;
    return 1;
  }
}
// remove a PROC with the highest priority (the first one in queue)
// returns its pointer
PROC *dequeue (PROC **queue) {
  PROC *p = *queue;
  //myprintf("dequeue()\n");
  //myprintf("queue->pid = %d\n", (*queue)->pid);
  //printProc(freeList);
  if(*queue) {
    *queue = (*queue)->next;
  }
  p->next = NULL;
  //  printProc(p);
  return p;
}

int exit(){
  if(running->pid == 0) {
    body();
  }
  else {
    running->status = ZOMBIE;
    tswitch();
  }
  return 0;
}
int printQueue(PROC *queue, char *queueName) {
  PROC *p = 0;
  //  myprintf("printQueue(%s)\n", queueName);
  p = queue;
  if(!p){
    myprintf("Queue is empty!\n");
    return 0;
  }
  while(p){
    myprintf("[%d, %d]->", p->pid, p->priority);
    p = p->next;
  }
  myprintf("NULL\n");
  return 1;
}
PROC *kfork() // create a child process, begin from body()
{
  int i;
  PROC *p = 0;
  
  p = get_proc(&freeList);
  //myprintf("kfork()\n");
  //myprintf("PROCESS RECEIVED:\n");
  //printProc(p);

  if(!p) {
    printf("no more PROC, kfork() failed\n");
    return 0;
  }

  p->status = READY;
  p->priority = 1;        //priority = 1 for all proc except P0
  p->ppid = running->pid; //parent = running
  p->parent = running;

  /* Initialize new proc's kstack[ ] */
  for (i = 1; i < 10; i++)          // saved CPU registers
    p->kstack[SSIZE - i] = 0; 

  p->kstack[SSIZE-1] = (int)body; // resume point = address of body()
  printQueue(freeList, "freeList");
  p->ksp = &(p->kstack[SSIZE-9]);   // proc saved sp
  enqueue(&readyQueue, p);        // enter p into readyQueue by priority

  myprintf("exit kfork()\n");
  return p;
}

int printProc(PROC *p) {
  if(p){
    myprintf("p:           %x\n", p);   
    myprintf("p->next:     %x\n", p->next);
    myprintf("p->ksp:      %x\n", p->ksp);
    myprintf("p->priority: %d\n", p->priority);
    myprintf("p->pid:      %d\n", p->pid);
    myprintf("p->ppid:     %d\n", p->ppid);
    myprintf("p->parent:   %x\n", p->parent);
    myprintf("p->kstack:   %x\n", p->kstack);
    getc();
  }
  else {
    myprintf("Process not found!\n");
    return 0;
  }
  return 1;
}

int init()
{
   PROC *p;
   int i, j;
   myprintf("intit()\n");
   /* initialize all proc's */
   for (i=0; i<NPROC; i++){
       p = &proc[i];
       p->status = FREE;
       p->pid = i;                        // pid = 0,1,2,..NPROC-1
       p->priority = 0;
       p->ppid = 0;
       p->parent = 0;
       if(i < NPROC - 1)
	 p->next = &proc[i+1];
       else
	 p->next = 0;
       if (i){                            // not for P0
          p->kstack[SSIZE-1] = (int)body; // entry address of body()
          for (j=2; j<10; j++)            // kstack[ ] high end entries = 0
               p->kstack[SSIZE-j] = 0;
          p->ksp = &(p->kstack[SSIZE-9]);
       }
       //printProc(p);
   }       
   running = &proc[0];
   running->status = READY;
   running->parent = &proc[0];
       
   
   proc[NPROC-1].next = NULL;             // all procs form a linear link list
   freeList = &proc[1];
   readyQueue = 0;

   printQueue(readyQueue, "readyQueue");
   myprintf("init complete\n");
 }

body()
{ 
  PROC *child = 0;
  char c = '\0';
  myprintf("proc %d resumes to body()\n", running->pid);
  while(c != 'q'){
    color = running->pid + 7;
    myprintf("proc %d running : enter a key[s|q|f|z|a|w] : ", running->pid);
    c = getc();
    myprintf("%c\n", c);
    //    printQueue(readyQueue,"");
    switch(c){
    case 's': do_tswitch();
      break;
    case 'q': 
      do_exit();
      break;
    case 'f': 
      do_kfork();
      break;
    case 'z' :
      break;
    case 'a' :
      break;
    case 'w' :
      break;
    }
  }
}

int main()
{
  myprintf("MTX starts in main()\n");
  init();
  kfork();
  tswitch();
  body();
}
