#!/usr/sbin/dtrace -s 
#pragma D option quiet

dtrace:::BEGIN
{
	self->ts = 0;
	self->preempt = 0;
	self->sleepTime = 0;
	sleepTime[tid] = 0;
}

sched:::on-cpu
/(execname == "omp" || execname == "spmv_v2")/
{
	printf("TID %d ON CPU %d\n", tid, cpu);
	self->ts = timestamp;
}

sched:::preempt 
/(execname == "omp" || execname == "spmv_v2")/
{
	self->preempt = 1;
	printf("TID %d was preempted\n", tid);
}

sched:::remain-cpu
/self->preempt == 1/
{
	self->preempt = 0;
	@rem[cpu] = count();
	printf("Thread %d was preempted but dispatcher has elected it to continue running\n", tid);
}

sched:::off-cpu
/(execname == "omp" || execname == "spmv_v2") && self->preempt == 1 && self->ts != 0/
{
	@pre[cpu] = count();
	@times[tid] = sum(timestamp - self->ts);
	printf("TID %d was preempted off CPU %d by %s\n", tid, cpu, args[1]->pr_fname);
	self->preempt = 0;
	self->ts = 0;
}

sched:::off-cpu
/(execname == "omp" || execname == "spmv_v2") && self->preempt == 0 && self->ts != 0/
{
	@times[tid] = sum(timestamp - self->ts);
	printf("TID %d OFF CPU %d\n", tid, cpu);
	self->ts = 0;
}

sched:::change-pri
/(execname == "omp" || execname == "spmv_v2") /
{
	@pri = count();
}

sched:::dequeue
/(execname == "omp" || execname == "spmv_v2") /
{
	@deq = count();
}

sched:::enqueue
/(execname == "omp" || execname == "spmv_v2") /
{
	@enq = count();
}

sched:::sleep
/(execname == "omp" || execname == "spmv_v2") /
{
	@sleep[cpu] = count();
	@sleepT[tid] = count();
	sleepTime[cpu] = timestamp;
}
sched:::wakeup
/(execname == "omp" || execname == "spmv_v2") && sleepTime[args[0]->pr_lwpid] != 0 /
{
	@sleepTimes[cpu] = sum(timestamp - sleepTime[args[0]->pr_lwpid]);
	sleepTime[args[0]->pr_lwpid] = 0;
}

dtrace:::END
{
	printf("\n\n---- CPU distribution ----\n\n");
	printa("Thread %d ran for %@d ns\n", @times);
	printa("Threads were preempted %@d times on CPU %d\n", @pre);
	printa("%@d threads were remained on cpu after preemption on CPU %d\n", @rem);
	printa("Threads slept %@d times on cpu %d\n", @sleep);
	printa("Thread %d slept %@d times \n", @sleepT);
	printa("Threads on CPU %d slept for %@d ns\n", @sleepTimes);
	printa("Total changes of priorities: %@d\n", @pri);
	printa("Total dequeues: %@d\n", @deq);
	printa("Total enqueues: %@d\n", @enq);
}
