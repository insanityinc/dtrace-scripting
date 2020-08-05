#!/usr/sbin/dtrace -s 

#pragma D option quiet

dtrace:::BEGIN
{
	printf("Tracing... Hit Ctrl-C to end.\n");
	x = 0;
	success = -1;
	self->time = 0;
}

syscall::write*:entry
/execname == "germ"/
{
	self->time = timestamp;
}
syscall::write*:return
/execname == "germ" && arg0 != -1/
{
	printf("\n%d bytes written\n", arg0);
	bytes[x] = arg0;
	x++;
	printf("Took %d ns to write all %d bytes to file\n", (timestamp - self->time), arg0);
	self->time = 0;
}

syscall::write*:return
/execname == "germ" && arg0 == -1/
{
	printf("Error writing to file!\n");	
}
syscall::read*:entry
/(execname == "sequential" || execname == "omp" || execname == "spmv_v2")/
{
	self->time = timestamp;
}
syscall::read*:return
/(execname == "sequential" || execname == "omp" || execname == "spmv_v2") && arg0 != -1 && (bytes[0] == arg0 || bytes[1] == arg0)/
{
	printf("\n%d bytes read\n", arg0);
	printf("Took %d ns to read all %d bytes to file\n", (timestamp - self->time), arg0);
	self->time = 0;
	success++;
}
syscall::read*:return
/(execname == "sequential" || execname == "omp" || execname == "spmv_v2") && arg0 == -1 && (bytes[0] == arg0 || bytes[1] == arg0)/
{
	printf("Error reading file!\n");
}
dtrace:::END
/success == 1/
{
	printf("All bytes were read from the files!"); 
}
dtrace:::END
/success != 1/
{
	printf("Error reading files");
}
