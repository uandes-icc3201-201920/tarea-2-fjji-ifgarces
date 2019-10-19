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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

int npages, nframes; //los totales
const char* policy;
const char* program; //  <fifo|rand> <seq|rand|rev> respectivamente
char *virtmem;
char *physmem;
int * fifo_entradas		= NULL; //son los accesos en el orden que se entro a fifo
int recien 				= 0; //ultimo dato que se coloco
int primer				= 0; //primer dato colocado

int * tabla_marcos		= NULL; //es la tabla de frames a usar para guiarse 
int * bit_tabla_marcos	= NULL;//para indicar el bit de cada frame de la tabla_marcos

int faltas_de_pagina 		= 0;
int remplazos_de_pagina		= 0;
int lecturas_realizadas		= 0;
int escrituras_realizadas	= 0;
struct disk *disk;

void FIFO_mode(struct page_table *pt,int page,int frame,int bits){
	//debemos ver si nos quedan frames para cargar la info
	int queda_frame	= -1; //partimos diciendo que no quedan frames
	for (int f=0;f<=nframes;f++){//recorremos los frames
		if (bit_tabla_marcos[f]==0){//si hay un frame disponible
			queda_frame = f;//queda_frame es el frame que encontramos libre
			break;
		}
	}	
	if (queda_frame==-1){//si no encontramos un frame
		escrituras_realizadas ++;
		disk_write(disk,tabla_marcos[fifo_entradas[recien]],&physmem[fifo_entradas[recien]*PAGE_SIZE]);
		page_table_set_entry(pt,tabla_marcos[fifo_entradas[recien]],fifo_entradas[recien],0); //vaciamos la pagina para llenarla
		bit_tabla_marcos[fifo_entradas[recien]] = 0; //vaciamos el bit asociado
		recien++;
		recien = recien % nframes; //evitemos errores por pasarnos
		lecturas_realizadas	++;
		disk_read(disk,page,&physmem[fifo_entradas[recien]*PAGE_SIZE]);
		fifo_entradas[primer] = fifo_entradas[recien];//dejamos al primer a eliminar con los datos del recien ingresado
		primer ++;
		primer = primer % nframes; //no pasarse
		tabla_marcos[fifo_entradas[recien]] = page;
		bit_tabla_marcos[fifo_entradas[recien]] = bits;//seteamos la tabla
		page_table_set_entry(pt,page,fifo_entradas[recien],bits);//lo pasamos a pt
	}
	else{//encontramos frame para usar
		lecturas_realizadas	++;
		bits = PROT_READ;//proteccion de lectura mode on
		disk_read(disk, page, &physmem[queda_frame*PAGE_SIZE]);
		tabla_marcos[frame] = page;
		bit_tabla_marcos[frame] = bits;//seteamos la tabla
		page_table_set_entry(pt,page,fifo_entradas[queda_frame],bits);//lo pasamos a pt
		
		
	}
}
void RAND_mode(struct page_table *pt,int page,int frame,int bits){
	srand(time(NULL)); 
	int random_f = rand() % nframes;
	int queda_frame	= -1; //partimos diciendo que no quedan frames
	for (int f=0;f<=nframes;f++){//recorremos los frames
		if (bit_tabla_marcos[f]==0){//si hay un frame disponible
			queda_frame = f;//queda_frame es el frame que encontramos libre
			break;
		}
	}
	if (queda_frame==-1){//si no encontramos un frame
		bits = PROT_READ;
		escrituras_realizadas ++;
		disk_write(disk,tabla_marcos[random_f],&physmem[random_f*PAGE_SIZE]);
		page_table_set_entry(pt,tabla_marcos[random_f],random_f,0); //vaciamos la pagina para llenarla
		lecturas_realizadas	++;
		disk_read(disk,page,&physmem[random_f*PAGE_SIZE]);
		tabla_marcos[random_f] = page;
		bit_tabla_marcos[random_f] = bits;//seteamos la tabla
		page_table_set_entry(pt,page,random_f,bits);//lo pasamos a pt
	
	}
	else{//encontramos frame para usar
		lecturas_realizadas	++;
		bits = PROT_READ;//proteccion de lectura mode on
		disk_read(disk, page, &physmem[queda_frame*PAGE_SIZE]);
		tabla_marcos[frame] = page;
		bit_tabla_marcos[frame] = bits;//seteamos la tabla
		page_table_set_entry(pt,page,queda_frame,bits);//lo pasamos a pt
		
	}
}

void page_fault_handler( struct page_table *pt, int page )
{
	faltas_de_pagina ++;
	int frame,bits;
	page_table_get_entry(pt, page, &frame, &bits); //me asigna el frame y el bit, aparteesta en consejos
	printf("page fault on page #%d\n",page);
	if (bits == 0){ //en enunciado mencionan que si bits son 0, no esta en memoria
		remplazos_de_pagina++;
		if (!strcmp(policy,"fifo")){
			FIFO_mode(pt,page,frame,bits);
		}
		else if (!strcmp(policy,"rand")){
			RAND_mode(pt,page,frame,bits);
		}
		else{
			printf("No conozco la politica de : %s\n", policy);
			exit(1);/*finalmente se cae por que no conozco el metodo dicho*/
		}
		}
	else{ //en caso que ya este en memoria 
		bits = PROT_READ | PROT_WRITE;
		tabla_marcos[frame] = page;
		bit_tabla_marcos[frame] =bits;//seteamos la tabla
		page_table_set_entry(pt,page,frame,bits);//lo pasamos a pt
		}

}

int main( int argc, char *argv[] )
{
	if(argc!=5) {
		printf("use: virtmem <npages> <nframes> <fifo|rand> <seq|rand|rev>\n");
		return 1;
	}

	npages	= atoi(argv[1]);
	nframes	= atoi(argv[2]);
	policy	= argv[3];
	program	= argv[4];

	tabla_marcos		= malloc(nframes*sizeof(int));
	bit_tabla_marcos	= malloc(nframes*sizeof(int));
	printf("Cantidad de páginas: %d\n", npages);
	printf("Cantidad de marcos: %d\n", nframes);
	printf("Algoritmo de reemplazo de página: %s\n", policy);
	printf("Patrón de acceso a memoria: %s\n", program);
	if  (!strcmp(policy,"fifo")){
		fifo_entradas = malloc( nframes * sizeof(int));
	}
	disk = disk_open("myvirtualdisk",npages);
	if(!disk) {
		fprintf(stderr,"couldn't create virtual disk: %s\n",strerror(errno));
		return 1;
	}


	struct page_table *pt = page_table_create( npages, nframes, page_fault_handler );
	if(!pt) {
		fprintf(stderr,"couldn't create page table: %s\n",strerror(errno));
		return 1;
	}

	virtmem = page_table_get_virtmem(pt);

	physmem = page_table_get_physmem(pt);

	if(!strcmp(program,"seq")) {
		access_pattern1(virtmem,npages*PAGE_SIZE);

	} else if(!strcmp(program,"rand")) {
		access_pattern2(virtmem,npages*PAGE_SIZE);

	} else if(!strcmp(program,"rev")) {
		access_pattern3(virtmem,npages*PAGE_SIZE);

	} else {
		fprintf(stderr,"no conozco el metodo de acceso: %s\n",argv[4]);

	}

	printf("Cantidad de faltas de página: %d\n", faltas_de_pagina);
	printf("Cantidad de reemplazos de página %d\n", remplazos_de_pagina);
	
	page_table_delete(pt);
	disk_close(disk);
	free(tabla_marcos);
	free(bit_tabla_marcos);
	free(fifo_entradas);

	return 0;
}
