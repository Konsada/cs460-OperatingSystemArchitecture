// ucode.c file

char *cmd[]={"getpid", "ps", "chname", "kfork", "switch", "wait", "exit", 0};

#define LEN 64

int show_menu()
{
   printf("***************** Menu *******************\n");
   printf("*  ps  chname  kfork  switch  wait  exit *\n");
   printf("******************************************\n");
}

int find_cmd(char *name)
{
  int i;
  for(i = 0; cmd[i]; i++){
    if(!strcmp(name, cmd[i]))
      return i;
  }
  return -1;
}

int getpid()
{
  //  printf("entering the kernel to get pid...\n");
   return syscall(0,0,0);
}

int ps()
{
  printf("entering the kernel to print processes...\n");
   return syscall(1, 0, 0);
}

int chname()
{
    char s[32];
    printf("input new name : ");
    gets(s);
    return syscall(2, s, 0);
}

int kfork()
{   
  int child, pid;
  pid = getpid();
  printf("proc %d enter kernel to kfork a child\n", pid); 
  child = syscall(3, 0, 0);
  if(child < 0)
    printf("kfork failed!\n");
  else
    printf("proc %d kforked a child %d\n", pid, child);
}    

int kswitch()
{
  int child;
  printf("proc %d entering kernel to switch process\n", getpid());
    return syscall(4,0,0);
    printf("proc %d back form kernel\n", getpid());
}

int wait()
{
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(5, &exitValue, 0);
    printf("proc %d back from wait, dead child=%d", getpid(), child);
    if (child>=0)
        printf("exitValue=%d", exitValue);
    printf("\n"); 
} 


int geti()
{
  // return an input integer
  char input[16];
  return atoi(gets(input));

}


int exit()
{
   int exitValue;
   char *input;
   printf("enter an exitValue: ");
   //   gets(input);
   //   exitValue = getint(input);
   exitValue = geti();
   printf("exitvalue=%d\n", exitValue);
   printf("enter kernel to die with exitValue=%d\n", exitValue);
   _exit(exitValue);
}

int _exit(int exitValue)
{
  return syscall(6,exitValue,0);
}

int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
}