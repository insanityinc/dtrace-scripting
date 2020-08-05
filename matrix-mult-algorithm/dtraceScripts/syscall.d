#!/usr/sbin/dtrace -s 

#pragma D option quiet

dtrace:::BEGIN
{
	printf("Tracing... Hit Ctrl-C to end.\n");
	self->time = 0;
}

syscall:::entry
/execname == "germ" || execname == "omp" || execname == "sequential" || execname == "spmv_v2"/
{
	self->time = timestamp;
}
syscall:::return
/self->time /
{
	@timesTotal[cpu, probefunc] = sum(timestamp - self->time);
	@timesAvg[cpu, probefunc] = avg(timestamp - self->time);
	@timesQuant[probefunc] = quantize(timestamp - self->time);
	@total[cpu, probefunc] = count();
	self->time = 0;

}
dtrace:::END
{
	printf("\n---- TOTAL TIME SPENT BY CPU ON EACH SYSCALL FUNCTION ----\n\n");
	printa("CPU %d spent in %s a total of %@d ns\n", @timesTotal);
	
	printf("\n---- AVERAGE TIME SPENT BY CPU ON EACH SYSCALL FUNCTION ----\n\n");
	printa("CPU %d spent in %s an average of %@d ns\n", @timesAvg);
	
	printf("\n---- TIME DISTRIBUITION ON EACH SYSCALL FUNCTION ----\n\n");
	printa(@timesQuant);

	printf("\n---- TOTAL CALLS FOR EACH SYSTEM CALL BY CPU ----\n\n");
	printa("CPU %d called %s %@d times\n", @total);
}
