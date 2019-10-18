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

int page_fault_count = 0, page_replace_count = 0;
struct disk* disk;
char* BUFFER;   // string donde se escribe-lee al disco.
int is_physmem_FULL;  // verdadero si pt->physmem está lleno (todos los frames ocupados)

unsigned int* tabla_marcos;  // segun Consejos
int en_memoria; //auxiliar que marca si estamos en memoria o no

int npages, nframes;
char* virtmem, physmem;
const char* policy;  // lru | fifo  (lru == rand?)
const char* pattern;  // antes "program" = pattern1|pattern2|pattern3

struct PhysMem_data
{
	int* Pages;  // contiene [índice de] páginas en memoria física. Rellena con -1s al principio.
	int PageCount;  // cantidad de páginas en memoria física.
} physical_memory;

void replace_page( struct page_table* pt, int pageIN, const char* elalgoritmo )
{
	page_replace_count++;
	
	if (! strcmp(elalgoritmo, "fifo"))
	{
		int fist_page = 0;  // buscando la primera página de la tabla de páginas que 
		for (int f = 0; f < nframes; f++)
		{
			if (physical_memory.Pages[f] != -1)  // encontró primera página que se agregó a memoria
			{
				fist_page = physical_memory.Pages[f];
				// guardar página en disco y colocar pageIN en physmem
				disk_write(disk, (PAGE_SIZE*fist_page)/BLOCK_SIZE, (char*)fist_page);
				page_table_get_physmem(pt)[f] = (char*)pageIN;
				physical_memory.Pages[f] = pageIN;
			}
		}
	}
	if (! strcmp(elalgoritmo, "rand"))  // == "lru" en el enunciado.
	{
		
	}
	else
	{
		printcolor(RED, "[!] Error, algoritmo de reemplazo de página inválido. Debe ser \'fifo\' o \'rand\'\n");
		exit(1);
	}
	
}


void page_fault_handler( struct page_table* pt, int page )
{   /// SE GATILLA AL QUERER ACCEDER A UNA PÁGINA QUE NO ESTÁ EN MEMORIA VIRTUAL (pt->virtmem) Y HAY QUE TRAERLA DESDE EL DISCO (disk)
	page_fault_count++;
color_start(RED);
	printf("page fault on page #%d\n", page);
color_end();
	
	int p_frame, p_bits, p_block;
	page_table_get_entry(pt, page, &p_frame, &p_bits);
	strcpy(BUFFER, "");
	
	for (int frameNum = 0; frameNum < nframes; frameNum++)
	{   // recorriendo marcos para ver si hay uno desocupado para poner la página directamente (no reemplazo de página)
		if (tabla_marcos[frameNum] == 0)   // encontró marco desocupado
		{
			strcpy(BUFFER, "");
			p_block = (PAGE_SIZE * page) / BLOCK_SIZE;
			disk_read(disk, p_block, BUFFER);
			page_table_get_physmem(pt)[frameNum*PAGE_SIZE] = BUFFER[0];  // poniendo página del disco en la physmem
			tabla_marcos[frameNum] = 1;
			physical_memory.PageCount++;
			return;
		}
	}
	// llega aquí si ni hay ningun marco disponible
	replace_page(pt, page, policy);
}


int main(int argc, char* argv[])
{
	physical_memory.Pages = (int*) malloc(sizeof(int)*nframes);
	for (int k = 0; k < nframes; k++) { physical_memory.Pages = -1; }
	// luego, si la página i está en memoria, ocurre que pages_in_PhysMem[i] != -1
	if (argc != 5)
	{
		printf("use: ./virtmem <npages> <nframes> <fifo|rand> <seq|rand|rev>\n");
		return 1;
	}
	
	npages  = atoi(argv[1]);
	nframes = atoi(argv[2]);
	policy  = argv[3];
	pattern = argv[4];
	tabla_marcos  = malloc(sizeof(int)*nframes);
	for (int k = 0; k < nframes; k++) { tabla_marcos[k] = 0; }
	BUFFER = malloc(sizeof(char)*200);
	
	disk = disk_open("myvirtualdisk", npages);   //struct disk* disk = disk_open("myvirtualdisk", npages);
	if (! disk)
	{
		fprintf(stderr, "couldn't create virtual disk: %s\n", strerror(errno));
		return 1;
	}
	struct page_table* pt = page_table_create( npages, nframes, page_fault_handler );
	if (! pt)
	{
		fprintf(stderr, "couldn't create page table: %s\n", strerror(errno));
		return 1;
	}
	
	virtmem = page_table_get_virtmem(pt);
	physmem = page_table_get_physmem(pt);

	if (! strcmp(pattern, "seq"))        // sequential
	{
		access_pattern1(virtmem, npages*PAGE_SIZE);
	}
	else if (! strcmp(pattern, "rand"))  // random
	{
		access_pattern2(virtmem, npages*PAGE_SIZE);
	}
	else if (! strcmp(pattern, "rev"))   // reverse
	{
		access_pattern3(virtmem, npages*PAGE_SIZE);
	}
	else
	{
		fprintf(stderr, "unknown pattern: %s\n", argv[3]);
	}
	
color_start(GREEN);
	printf("[TEST] Page Table status: \n");
	printf(" fd\t virtmem\t npages\t physmem\t nframes\t page_mapping\t page_bits\t\n");
	//printf(" %d\t %s\t %d\t %s\t %d\t ", pt->fd, pt->virtmem, pt->npages, pt->physmem, pt->nframes);
	//printf(" ??\t \'%s\'\t %d\t \'%s\'\t %d\t ??\t ??\n", virtmem, npages, physmem, nframes);
	printf(" ??\t %s\t %d\t \'%s\'\t %d   \t ??\t ??\n", "<protected>", npages, page_table_get_physmem(pt), nframes);
	
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
	
	printf("Cantidad de faltas de página: %d\n", page_fault_count);
	printf("Cantidad de reemplazos de página %d\n", page_replace_count);
	
	page_table_delete(pt);
	disk_close(disk);
	free(BUFFER);
	return 0;
}
