#!/usr/sbin/dtrace -s 
#pragma D option quiet
dtrace:::BEGIN
{
	self->time = 0;
}

proc:::create
/(execname == "omp" || execname == "spmv_v2")/
{ 
	printf("Process created\n");
	mainPid = pid;
}

proc:::start
/(execname == "omp" || execname == "spmv_v2") && pid != mainPid/
{ 
	self->time = timestamp;
	@totalStarts = count();
	printf("Process %d started\n", pid);
}

proc:::exit
/(execname == "omp" || execname == "spmv_v2") && pid != mainPid/
{
	self->final = (timestamp - self->time);
	@totalExits = count();
	@timesAvg = avg(self->final);
	printf("process %d is exiting after %d ns\n", pid, self->final);
	self->time = 0;
}

proc:::fault 
/(execname == "omp" || execname == "spmv_v2")/
{
	printf("process %d experienced machine fault with signal number %d, signal code %d and error number %d\n", pid, args[1]->si_signo, args[1]->si_code, args[1]->si_errno);
}


dtrace:::END
{
	printa("Each process computed for an average of %@d\n", @timesAvg);
	printa("%@d process were successfully created by the main process\n", @totalStarts);
	printa("%@d process created successfully exited\n", @totalExits);
}

