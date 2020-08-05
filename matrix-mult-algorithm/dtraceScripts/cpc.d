#!/usr/sbin/dtrace -s 
#pragma D option quiet

dtrace:::BEGIN
{
	start = timestamp;
	self->mult = 0;
}

cpc:::PAPI_l1_dcm-user-10000
/(execname == "omp" || execname == "sequential" || execname == "spmv_v2") /
{
	printf("L1 cache miss!\n");
	@[execname, ufunc(arg1)] = quantize(1);
}

dtrace:::END
{
	printf("\n ---- L1 CACHE MISSES ----\n");
}
