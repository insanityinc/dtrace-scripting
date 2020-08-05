#!/usr/sbin/dtrace -s

#pragma D option quiet

BEGIN {

  printf("Tracing... Press Ctrl-C to end\n\n\n");
  self->crt = -1;
}
syscall::openat*:entry
/ (arg2 & O_CREAT) == 0 /
{
  @total[pid, execname] = count();
  self->crt = 0;
}
syscall::openat*:entry
/ (arg2 & O_CREAT) == O_CREAT /
{
  @totalCreate[pid, execname] = count();
  self->crt = 1;
}
syscall::openat*:return
/ arg0 != -1 && self->crt == 1/
{
  @totalSuccessCRT[pid, execname] = count();
}
syscall::openat*:return
/ arg0 != -1 && self->crt == 0/
{
  @totalSuccessOPN[pid, execname] = count();
}

tick-$1s {

  printf("\nCurrent time: %Y\n", walltimestamp);
  printf("\nTotal attempts to open already created files\n");
  printf("     PID - EXECNAME");
  printa (@total);
  printf("\nTotal successful attempts to open already created files\n");
  printf("     PID - EXECNAME");
  printa (@totalSuccessOPN);
  printf("\nTotal attempts to create files\n");
  printf("     PID - EXECNAME");
  printa (@totalCreate);
  printf("\nTotal successful attempts to create files\n");
  printf("     PID - EXECNAME");
  printa (@totalSuccessCRT);
}

END {
  printf("\nTracing interrupted!\n");
  printf("\nCurrent time: %Y\n", walltimestamp);
  printf("\nTotal attempts to open already created files\n");
  printf("     PID - EXECNAME");
  printa (@total);
  printf("\nTotal successful attempts to open already created files\n");
  printf("     PID - EXECNAME");
  printa (@totalSuccessOPN);
  printf("\nTotal attempts to create files\n");
  printf("     PID - EXECNAME");
  printa (@totalCreate);
  printf("\nTotal successful attempts to create files\n");
  printf("     PID - EXECNAME");
  printa (@totalSuccessCRT);
}
