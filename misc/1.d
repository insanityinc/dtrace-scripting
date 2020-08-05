#!/usr/sbin/dtrace -s 

#pragma D option quiet

dtrace:::BEGIN
{
	printf("Tracing... Hit Ctrl-C to end.\n");
}

syscall::openat*:entry
{
	self->path = copyinstr(arg1);
	self->flags = arg2;
}

syscall::openat*:return
/ strstr(self->path, "/etc/") != NULL/
{
	printf("EXECNAME: %s; PID: %d; UID: %d; GID: %d; PATH: %s; ", execname, pid, uid, gid, self->path);
	printf("FLAG: %s", self->flags & O_WRONLY ? "O_WRONLY" : self->flags & O_RDWR ? "O_RDWR" : "O_RDONLY");
	printf(" %s", self->flags & O_APPEND ? " | O_APPEND" : "");
	printf(" %s; ", self->flags & O_CREAT ? " | O_CREAT" : "");
	printf("RETURN VALUE: %d\n", arg1); 
}

