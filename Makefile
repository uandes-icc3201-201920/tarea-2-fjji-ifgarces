FLAGS = -Wall -g
# la flag -g genera una especie de reporte de error si es que lo hay (al ejecutar?)
EXE = virtmem
diskname = myvirtualdisk
CC = gcc

$(EXE): *.c *.h Makefile clean ReDraw
	$(CC) -o colormaster.o ColorMaster.c -c $(FLAGS)
	$(CC) -o page_table.o page_table.c -c $(FLAGS)
	$(CC) -o disk.o disk.c -c $(FLAGS)
	$(CC) -o program.o program.c -c $(FLAGS)
	$(CC) -o main.o main.c -c $(FLAGS)
	$(CC) -o $(EXE) main.o program.o disk.o page_table.o colormaster.o $(FLAGS)

ReDraw:
	clear;reset

clean:
	rm -f $(EXE) *.o *.tar.gz $(diskname)

run:
	# ./virtmem <npages> <nframes> fifo|rand seq|rand|rev
	./$(EXE) 50 20 fifo rand

compress:
	tar -zcvf TAREA2_fjji_ifgarces.tar.gz Makefile *.c *.h README.md informe-t2.docx