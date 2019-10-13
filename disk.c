/*
Do not modify this file.
Make all of your changes to main.c instead.
*/

//#include "disk.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

extern ssize_t pread(int __fd, void* __buf, size_t __nbytes, __off_t __offset);
extern ssize_t pwrite(int __fd, const void* __buf, size_t __nbytes, __off_t __offset);


struct disk {
	int fd;
	int block_size;
	int nblocks;
};

struct disk* disk_open( const char* diskname, int nblocks )
{
	struct disk* d;

	d = malloc(sizeof(*d));    // (struct disk*)malloc(sizeof(struct disk))
	if (!d) return 0;

	d->fd = open(diskname, O_CREAT|O_RDWR, 0777);
	if (d->fd < 0)
	{
		free(d);
		return 0;
	}

	d->block_size = BLOCK_SIZE;
	d->nblocks = nblocks;

	if (ftruncate( d->fd, (d->nblocks)*(d->block_size) ) < 0)     // ftruncate borra el contenido del disco. Returna 0 si tiene éxito y -1 si hubo error.
	{
		close(d->fd);
		free(d);
		return 0;
	}

	return d;
}

void disk_write( struct disk* d, int block, const char* data )
{
	if (block < 0 || block >= d->nblocks)
	{
		fprintf(stderr, "disk_write: invalid block #%d\n", block);
		abort();   // abort() es como exit() pero no elimina buffer ni cierra todos los archivos abiertos (...)
	}

	int actual = pwrite(d->fd, data, d->block_size, block*d->block_size);
	if (actual != d->block_size)   // ¿por qué? ¿qué problema hay que escriba menos bytes? Hay error cuando actual == -1.
	{
		fprintf(stderr, "disk_write: failed to write block #%d: %s\n", block, strerror(errno));
		abort();
	}
}

void disk_read( struct disk* d, int block, char* data )
{
	if(block < 0 || block >= d->nblocks)
	{
		fprintf(stderr, "disk_read: invalid block #%d\n", block);
		abort();
	}

	int actual = pread(d->fd, data, d->block_size, block*d->block_size);
	if (actual != d->block_size)   // ¿qué hay de malo con que lea menos bytes?
	{
		fprintf(stderr, "disk_read: failed to read block #%d: %s\n", block, strerror(errno));
		abort();
	}
}

int disk_nblocks( struct disk* d )
{
	return d->nblocks;
}

void disk_close( struct disk* d )
{
	close(d->fd);
	free(d);
}
