#!/usr/sbin/dtrace -qs

#pragma D option quiet

BEGIN {
  printf("Tracing...\nPress Ctrl-C to end\n");
}

syscall:::entry
/pid == $target/
{
 self->start_t = timestamp;
}

syscall:::return
/pid == $target && self->start_t/
{
 @syscalls[probefunc] = count();
 @timespent[probefunc] = sum(timestamp - self->start_t);
 self->start_t = 0;
}

END
{
 printa("Total system calls for probe function %s: %@d\n", @syscalls);
 printa("Time spent on system function %s : %@d ns\n", @timespent);
}
