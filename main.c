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

int* tabla_marcos;  // segun Consejos
// int en_memoria;  [sin usar] //auxiliar que marca si estamos en memoria o no


int frame, bits, npages, nframes;

char* virtmem;
char* physmem;
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
		int first_page = 0;  // buscando la primera página de la tabla de páginas que 
		for (int f = nframes; f >= 0; f--)
		{
			if (physical_memory.Pages[f] != -1)  // encontró primer frame en memoria ocupado con una página
			{
				first_page = physical_memory.Pages[f];
				// ahora guardar página en disco y colocar pageIN en physmem
				sprintf(BUFFER, "%c", page_table_get_physmem(pt)[first_page*PAGE_SIZE]);  // usando esta vez BUFFER como auxiliar de forma distinta
				const char* _aux = BUFFER;
				disk_write(disk, (PAGE_SIZE*first_page)/BLOCK_SIZE, _aux);
				
				physmem[f] = (char)pageIN;
				physical_memory.Pages[f] = pageIN;
				page_table_set_entry(pt, first_page, f, 0);
				return;
			}
		}
	}
	if (! strcmp(elalgoritmo, "rand"))  // == "lru" en el enunciado.
	{
		int iterations, random_f, my_page;
		while (1)  // bucle hasta que encuentra frame ocupado con una página
		{
			random_f = rand()%nframes;
			if (physical_memory.Pages[random_f] != -1)   // cuando encuentra frame con una página, no vacío
			{
				my_page = physical_memory.Pages[random_f];
				sprintf(BUFFER, "%c", physmem[my_page*PAGE_SIZE]);  // usando esta vez BUFFER como auxiliar de forma distinta
				const char* _aux = BUFFER;
				disk_write(disk, (PAGE_SIZE*my_page)/BLOCK_SIZE, _aux);
				
				physmem[my_page] = (char)pageIN;
				physical_memory.Pages[my_page] = pageIN;
				page_table_set_entry(pt, my_page, random_f, 0);
				return;
			}
			
			if (iterations == 999999) { printf("[!] Error: se quedó pegado en función replace_page. (~ 1M iteraciones)\n"); exit(1); }
			iterations++;
		}
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
	//page_table_set_entry(pt, page, &frame, &bits); //maybe
	int p_frame, p_bits, p_block;
	page_table_get_entry(pt, page, &p_frame, &p_bits);
	strcpy(BUFFER, "");
	for (int frameNum = 0; frameNum < nframes; frameNum++)
	{   // recorriendo marcos para ver si hay uno desocupado para poner la página directamente (no reemplazo de página)
		if (tabla_marcos[frameNum] == -1)   // encontró marco desocupado
		{
			strcpy(BUFFER, "");
			p_block = (PAGE_SIZE * page) / BLOCK_SIZE;  // ??!!
			disk_read(disk, p_block, BUFFER);
			physmem[frameNum*PAGE_SIZE] = BUFFER[0];  // poniendo página del disco en la physmem
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
	if (argc != 5)
	{
		printf("use: ./virtmem <npages> <nframes> fifo|rand seq|rand|rev\n");
		return 1;
	}
	
	npages  = atoi(argv[1]);
	nframes = atoi(argv[2]);
	policy  = argv[3];
	pattern = argv[4];
	
	printf("\n\nCantidad de páginas: %d\n", npages);
	printf("Cantidad de marcos: %d\n", nframes);
	printf("Algoritmo de reemplazo de página: %s\n", policy);
	printf("Patrón de acceso a memoria: %s\n", pattern);
	
	tabla_marcos  = malloc(sizeof(int)*nframes);
	for (int k = 0; k < nframes; k++) { tabla_marcos[k] = -1; }
	BUFFER = malloc(sizeof(char)*200);
	
	physical_memory.Pages = (int*) malloc(sizeof(int)*nframes);
	for (int k = 0; k < nframes; k++) { physical_memory.Pages[k] = -1; } // luego, si la página i está en memoria, ocurre que pages_in_PhysMem[i] != -1

	
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
	physmem = (char*) malloc(sizeof(char)*5000);
	physmem = page_table_get_physmem(pt);  // [!!!] por qué da warning??? dice que es un integer??? wtf??? ya no hay warning si le pongo malloc, aunque parece que eso causa en algún punto segmentation fault.

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
		exit(1);
	}
	
/* color_start(BLUE);
	printf("[TEST] Page Table status: \n");
	printf(" fd\t virtmem\t npages\t physmem\t nframes\t page_mapping\t page_bits\t\n");

	printf(" ??\t %s\t %d\t \'%s\'\t %d   \t ??\t ??\n", "<protected>", npages, physmem, nframes);
color_end(); */
	
	printf("Cantidad de faltas de página: %d\n", page_fault_count);
	printf("Cantidad de reemplazos de página %d\n", page_replace_count);
	
	page_table_delete(pt);
	disk_close(disk);
	free(BUFFER);
	free(physmem);
	free(physical_memory.Pages);
	free(tabla_marcos);
	return 0;
}
