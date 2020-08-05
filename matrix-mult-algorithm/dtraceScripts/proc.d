#!/usr/sbin/dtrace -s 
#pragma D option quiet
dtrace:::BEGIN
{
	self->time = 0;
}

proc:::lwp-create
/(execname == "omp" || execname == "spmv_v2")/
{ 
	printf("thread created\n");
	mainTid = tid;
}

proc:::lwp-start
/(execname == "omp" || execname == "spmv_v2") && tid != mainTid/
{ 
	self->time = timestamp;
	@totalStarts = count();
	printf("thread %d started\n", tid);
}

proc:::lwp-exit
/(execname == "omp" || execname == "spmv_v2") && tid != mainTid/
{
	self->final = (timestamp - self->time);
	@totalExits = count();
	@timesAvg = avg(self->final);
	printf("thread %d is exiting after %d ns\n", tid, self->final);
	self->time = 0;
}

proc:::fault 
/(execname == "omp" || execname == "spmv_v2")/
{
	printf("thread %d experienced machine fault with signal number %d, signal code %d and error number %d\n", tid, args[1]->si_signo, args[1]->si_code, args[1]->si_errno);
}


dtrace:::END
{
	printa("Each thread computed for an average of %@d\n", @timesAvg);
	printa("%@d threads were successfully created by the main thread\n", @totalStarts);
	printa("%@d threads created successfully exited\n", @totalExits);
}

