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
#include "ColorMaster.h"
int BLACK = 30, RED = 31, GREEN = 32, YELLOW = 33, BLUE = 34, MAGENTA = 35, PURPLE = 35, CYAN = 36, WHITE = 37;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

int* free_frames;   // array de índices de marcos libres
struct disk* disk;
char* BUFFER;

void page_fault_handler( struct page_table* pt, int page )
{   // SE GATILLA AL QUERER ACCEDER A UNA PÁGINA QUE NO ESTÁ EN MEMORIA VIRTUAL (pt->virtmem) Y HAY QUE TRAERLA DESDE EL DISCO (disk)
	color_start(RED);
	printf("page fault on page #%d\n", page);
	color_end();
	
	BUFFER = malloc(sizeof(char)*40);
	strcpy(BUFFER, "");
	disk_read(disk, disk->block_size*?, BUFFER);
	
	exit(1);
}

int main(int argc, char* argv[])
{
	if (argc != 5)
	{
		printf("use: ./virtmem <npages> <nframes> <lru|fifo> <pattern1|pattern2|pattern3>\n");
		return 1;
	}

	int npages = atoi(argv[1]);
	int nframes = atoi(argv[2]);
	const char* policy = argv[3];
	const char* pattern = argv[4];  // antes llamado "program"
	
	free_frames = malloc(sizeof(int)*nframes);

	//struct disk* disk = disk_open("myvirtualdisk", npages);
	disk = disk_open("myvirtualdisk", npages);
	if (! disk)
	{
		fprintf(stderr, "couldn't create virtual disk: %s\n", strerror(errno));
		return 1;
	}

	struct page_table* pt = page_table_create( npages, nframes, page_fault_handler );
	if (! pt)
	{
		fprintf(stderr,"couldn't create page table: %s\n",strerror(errno));
		return 1;
	}

	char* virtmem = page_table_get_virtmem(pt);

	char* physmem = page_table_get_physmem(pt);

	if (! strcmp(pattern, "pattern1"))
	{
		access_pattern1(virtmem, npages*PAGE_SIZE);
	}
	
	else if (! strcmp(pattern, "pattern2"))
	{
		access_pattern2(virtmem, npages*PAGE_SIZE);
	}
	
	else if (! strcmp(pattern, "pattern3"))
	{
		access_pattern3(virtmem, npages*PAGE_SIZE);
	}
	
	else
	{
		fprintf(stderr, "unknown pattern: %s\n", argv[3]);
	}
	

color_start(BLUE);
	printf("[TEST] Page Table status: \n");
	printf(" fd\t virtmem\t npages\t physmem\t nframes\t page_mapping\t page_bits\t\n");
	printf(" %d\t %s\t %d\t %s\t %d\t ", pt->fd, pt->virtmem, pt->npages, pt->physmem, pt->nframes);
	unsigned int k;
	
	for (k = 0; k < sizeof(pt->page_mapping)/sizeof(int); k++)
	{
		printf("%d", pt->page_mapping[k]);
	}
	printf("\t ");
	for (k = 0; k < sizeof(pt->page_bits)/sizeof(int); k++)
	{
		printf("%d", pt->page_bits[k]);
	}
color_end();
	
	page_table_delete(pt);
	disk_close(disk);

	return 0;
}
