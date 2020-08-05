#!/usr/sbin/dtrace -s 
#pragma D option quiet

dtrace:::BEGIN
{
	printf("Tracing...");
	self->compute = 0;
}

pid$target::compute_multiplication:entry
{
	self->compute= 1;
}

lockstat:::adaptive-acquire
/ (execname == "omp" || execname == "spmv_v2") && self->compute == 1/
{
	@mutexacquire[cpu] = count();
}

lockstat:::adaptive-block
/ (execname == "omp" || execname == "spmv_v2") && self->compute == 1/
{
	@mutexblock[cpu] = count(); 
	@mutexblockTime[cpu] = sum(arg1/1000000000);
	@mutexblockTimeAvg[cpu] = avg(arg1/1000000000);
	@mutexblockTimeQuantize[cpu] = quantize(arg1/1000000000);
}
lockstat:::adaptive-release
/ (execname == "omp" || execname == "spmv_v2") && self->compute == 1/
{
	@mutexexit[cpu] = count();
}

lockstat:::rw-acquire
/ (execname == "omp" || execname == "spmv_v2") && self->compute == 1/
{
	@rwacquire[cpu] = count();
}
lockstat:::rw-block
/ (execname == "omp" || execname == "spmv_v2") && self->compute == 1/
{
	@rwblock[cpu] = count();
	@rwblockTime[cpu] = sum(arg1/1000000000);
	@rwblockTimeAvg[cpu] = avg(arg1/1000000000);
	@rwblockTimeQuantize[cpu] = quantize(arg1/1000000000);
	printf("Aqcuired blocked lock as %d", arg2);
}
lockstat:::rw-release
/ (execname == "omp" || execname == "spmv_v2") && self->compute == 1/
{
	@rwrelease[cpu] = count();
}
dtrace:::END
{
	printf("----Adaptive Locks----");
	printf("\n");
	printa("CPU %-10d acquired %10@d adaptive locks\n", @mutexacquire);
	printf("\n");
	printa("CPU %-10d acquired %10@d blocked adaptive locks\n", @mutexblock);
	printf("\n");
	printa("CPU %-10d slept %10@d seconds to acquire blocked adaptive lock", @mutexblockTime);
	printf("\n");
	printa("CPU %-10d slept an average of %10@d seconds to acquire each blocked adaptive lock", @mutexblockTime);
	printf("\n");
	printa("CPU %-10d released %10@d adaptive locks\n", @mutexexit);

	printf("----Readers/Writer Locks----");
	printf("\n");
	printa("CPU %-10d acquired %10@d rw locks\n", @rwacquire);
	printf("\n");
	printa("CPU %-10d acquired %10@d blocked rw locks\n", @rwblock);
	printf("\n");
	printa("CPU %-10d slept %10@d seconds to acquire blocked rw lock\n", @rwblockTime);
	printf("\n");
	printa("CPU %-10d slept an average of %10@d seconds to acquire each blocked rw lock\n", @rwblockTimeAvg);
	printf("\n");
	printa("CPU %-10d released %10@d rw locks\n", @rwrelease);
	printf("\n");

}
