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

int* free_frames;   // array con índices de marcos libres
struct disk* disk;
char* BUFFER;   // string donde se escribe-lee al disco.
unsigned int*  tabla_marcos; 
char* msj;  // auxiliar
int en_memoria; //auxiliar que marca si estamos en memoria o no

void page_fault_handler( struct page_table* pt, int page )
{   // SE GATILLA AL QUERER ACCEDER A UNA PÁGINA QUE NO ESTÁ EN MEMORIA VIRTUAL (pt->virtmem) Y HAY QUE TRAERLA DESDE EL DISCO (disk)
	sprintf(msj, "page fault on page #%d\n", page);
	printcolor(RED, msj);
	// page_table_get_entry(pt, (PAGE_SIZE*page)/BLOCK_SIZE, free_frames[page], en_memoria);
//[FJJI]Habria que setear en (pt, page, -algo- (posiblemente su frame actual) , Prot_Read|Prot_write  (visto por los bits de proteccion, como los tomo?)  ) ,de ahi hacer un disk_read que esta abajo
	BUFFER = malloc(sizeof(char)*40);
	strcpy(BUFFER, "");
	disk_read(disk, (PAGE_SIZE*page)/BLOCK_SIZE, BUFFER);   // verificar segundo arg
	    // [??] Cómo sé cuál bloque del disco leer? Cómo obtengo el bloque en el que está la página "page"?
		// [FJJI]BUFER segun lo que leo debe ser &physmem[n°frame * frame_size]
	exit(1);
}

void replace_page( struct page_table* pt, int page, const char* mode )
{
	if (! strcmp(mode, "fifo"))
	{
		
	}
	if (! strcmp(mode, "lru"))
	{
		
	}
	else
	{
		printcolor(RED, "[!] Error, política de algoritmo de reemplazo de página inválida. Debe ser \'fifo\' o \'lru\'\n");
		exit(1);
	}
}


int npages;
int nframes;
const char* policy;  // lru | fifo  (lru == rand?)
const char* pattern;  // antes "program" = pattern1|pattern2|pattern3

int main(int argc, char* argv[])
{
	if (argc != 5)
	{
		printf("use: ./virtmem <npages> <nframes> <lru|fifo> <pattern1|pattern2|pattern3>\n");
		return 1;
	}
	
	npages = atoi(argv[1]);
	nframes = atoi(argv[2]);
	policy = argv[3];
	pattern = argv[4];
	tabla_marcos  = malloc(sizeof(int)*npages);
	
	free_frames = malloc(sizeof(int)*nframes);
	msj = malloc(sizeof(char)*300);
	
	disk = disk_open("myvirtualdisk", npages);   //struct disk* disk = disk_open("myvirtualdisk", npages);
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
	//printf(" %d\t %s\t %d\t %s\t %d\t ", pt->fd, pt->virtmem, pt->npages, pt->physmem, pt->nframes);
	//printf(" ??\t \'%s\'\t %d\t \'%s\'\t %d\t ??\t ??\n", virtmem, npages, physmem, nframes);
	printf(" ??\t <protected>\t %d\t \'%s\'\t %d   \t ??\t ??\n", npages, physmem, nframes);
	
	/* unsigned int k;
	for (k = 0; k < sizeof(pt->page_mapping)/sizeof(int); k++)
	{
		printf("%d,", pt->page_mapping[k]);
	}
	printf("\t ");
	for (k = 0; k < sizeof(pt->page_bits)/sizeof(int); k++)
	{
		printf("%d,", pt->page_bits[k]);
	} */
color_end();

	page_table_delete(pt);
	disk_close(disk);

	return 0;
}
