/*
Main program for the virtual memory project.
Make all of your modifications to this file.
You may add or rearrange any code or data as you need.
The header files page_table.h and disk.h explain
how to use the page table and disk interfaces.
*/

#include "page_table.h"
#include "disk.h"
#include "program.h"
//#include "ColorMaster.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

void page_fault_handler( struct page_table* pt, int page )
{
	printf("page fault on page #%d\n",page);
	//... rellenar aquí
	exit(1);
}

int main( int argc, char* argv[] )
{
	if (argc != 5)
	{
		printf("use: virtmem <npages> <nframes> <lru|fifo> <access pattern>\n");
		return 1;
	}

	int npages = atoi(argv[1]);
	int nframes = atoi(argv[2]);
	const char* program = argv[4];

	struct disk* disk = disk_open("myvirtualdisk",npages);
	if (!disk)
	{
		fprintf(stderr,"couldn't create virtual disk: %s\n",strerror(errno));
		return 1;
	}


	struct page_table* pt = page_table_create( npages, nframes, page_fault_handler );
	if (!pt)
	{
		fprintf(stderr,"couldn't create page table: %s\n",strerror(errno));
		return 1;
	}

	char* virtmem = page_table_get_virtmem(pt);

	char* physmem = page_table_get_physmem(pt);

	if (!strcmp(program,"pattern1"))
	{
		access_pattern1(virtmem,npages*PAGE_SIZE);
	}
	
	else if (!strcmp(program,"pattern2"))
	{
		access_pattern2(virtmem,npages*PAGE_SIZE);
	}
	
	else if (!strcmp(program,"pattern3"))
	{
		access_pattern3(virtmem,npages*PAGE_SIZE);
	}
	
	else
	{
		fprintf(stderr,"unknown program: %s\n",argv[3]);
	}

	
	/* printf("Page Table status: \n");
	printf(" fd\t virtmem\t npages\t physmem\t nframes\t page_mapping\t page_bits\t\n");
	printf(" %d\t %s\t %d\t %s\t %d\t ", pt->fd, pt->virtmem, pt->npages, pt->physmem, pt->nframes);
	unsigned int k;
	
	for (k = 0; k < (sizeof(pt->page_mapping)/sizeof(int)); k++)
	{
		printf("%d", pt->page_mapping[k]);
	}
	printf("\t ");
	for (k=0; k<sizeof(pt->page_bits)/sizeof(int); k++)
	{
		printf("%d", pt->page_bits[k]);
	} */
	
	page_table_delete(pt);
	disk_close(disk);

	return 0;
}
