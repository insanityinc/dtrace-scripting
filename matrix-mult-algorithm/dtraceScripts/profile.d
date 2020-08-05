#!/usr/sbin/dtrace -s 
#pragma D option quiet

dtrace:::BEGIN
{
	start = timestamp;
	state[tid] = 'l';
}

profile-97
/pid == $target/
{
	@proc[pid, tid] = count();
	@procPri = lquantize(curlwpsinfo->pr_pri, 0, 100, 10);
}

profile-97
/pid == $target && state[tid] != curlwpsinfo->pr_sname/
{
	printf("Thread %d state: %c\n", tid, curlwpsinfo->pr_sname);
	state[tid] = curlwpsinfo->pr_sname;
}
END
{
	printf("%-8s %-40s %s\n", "PID", "TID", "COUNT");
	printa("%-8d %-40d %@d\n", @proc);
	printa(@procPri);
}
