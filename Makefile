FLAGS = -Wall -g
# la flag -g genera una especie de reporte de error si es que lo hay (al ejecutar?)
EXE = virtmem
diskname = myvirtualdisk

$(EXE): *.o *.c Makefile clean
	gcc -o colormaster.o ColorMaster.c -c $(FLAGS)
	gcc -o page_table.o page_table.c -c $(FLAGS)
	gcc -o disk.o disk.c -c $(FLAGS)
	gcc -o program.o program.c -c $(FLAGS)
	gcc -o main.o main.c -c $(FLAGS)
	gcc -o $(EXE) main.o program.o disk.o page_table.o colormaster.o $(FLAGS)

clean:
	rm -f $(EXE) *.o *.tar.gz $(diskname)

run:
	./$(EXE) 50 20 fifo pattern1

compress:
	tar -zcvf TAREA2_fjji_ifgarces.tar.gz Makefile *.c *.h README.md informe-t2.pdf