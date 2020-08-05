#!/usr/sbin/dtrace -s 
#pragma D option quiet

dtrace:::BEGIN
{
	printf("Tracing started...");
}

pid$target::clearCache:entry
{
	printf("Clearing Cache\n");
	@clear[cpu] = count();
	self->time = timestamp;
}
pid$target::clearCache:return
{
	printf("Cache cleared\n");
	@clearTimes[cpu] = sum((timestamp - self->time) / 1000000);
}
pid$target::read_matrix:entry
{
	printf("reading Matrix from file\n");
	@readMatrix[cpu] = count();
	self->time = timestamp;
}
pid$target::read_matrix:return
{
	printf("reading Matrix from file finished!\n");
	@readMatrixTimes[cpu] = sum((timestamp - self->time) / 1000000);
}
pid$target::read_vector:entry
{
	printf("reading vector from file\n");
	@readVector[cpu] = count();
	self->time = timestamp;
}
pid$target::read_vector:return
{
	printf("reading vector from file finished!\n");
	@readVectorTimes[cpu] = sum((timestamp - self->time) / 1000000);
}
pid$target::free_everything:entry
{
	printf("freeing everything\n");
	@free[cpu] = count();
	self->time = timestamp;
}
pid$target::free_everything:return
{
	printf("freed everything\n");
	@freeTimes[cpu] = sum((timestamp - self->time) / 1000000);
}
pid$target::count_non_zeros:entry
{
	printf("Counting zeros\n");
	@zeros[cpu] = count();
	self->time = timestamp;
}
pid$target::count_non_zeros:return
{
	printf("Zeros counted!\n");
	@zerosTimes[cpu] = sum((timestamp - self->time) / 1000000);
}
pid$target::create_sparce_matrix:entry
{
	printf("creating sparce matrix\n");
	@createSparce[cpu] = count();
	self->time = timestamp;
}
pid$target::create_sparce_matrix:return
{
	printf("Sparce matrix created!\n");
	@createSparceTimes[cpu] = sum((timestamp - self->time) / 1000000);
}
pid$target::check_solution:entry
{
	printf("checking solution\n");
	@check[cpu] = count();
	self->time = timestamp;
}
pid$target::check_solution:return
{
	printf("Solution checked!\n");
	@checkTimes[cpu] = sum((timestamp - self->time) / 1000000);
}
pid$target::compute_multiplication:entry
{
	printf("computing multiplication\n");
	@compute[cpu] = count();
	self->time = timestamp;
}
pid$target::compute_multiplication:return
{
	printf("multiplication computed!\n");
	@computeTimes[cpu] = sum((timestamp - self->time) / 1000000);
}

dtrace:::END
{
	printf("CPU - COUNT\n");
	printf("compute\n");
	printa(@compute);
	printa(@computeTimes);
	printf("check\n");
	printa(@check);
	printa(@checkTimes);
	printf("create sparce matrix\n");
	printa(@createSparce);
	printa(@createSparceTimes);
	printf("count non zeros\n");
	printa(@zeros);
	printa(@zerosTimes);
	printf("free everything\n");
	printa(@free);
	printa(@freeTimes);
	printf("Clear cache\n");
	printa(@clear);
	printa(@clearTimes);
	printf("read matrix\n");
	printa(@readMatrix);
	printa(@readMatrixTimes);
	printf("read vector\n");
	printa(@readVector);
	printa(@readVectorTimes);
}
