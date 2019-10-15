FLAGS = -Wall -g
# la flag -g genera una especie de reporte de error si es que lo hay (al ejecutar o compilar?)
EXENAME = virtmem

$(EXENAME): *.o *.c Makefile
	gcc -o colormaster.o ColorMaster.c -c $(FLAGS)
	gcc -o page_table.o page_table.c -c $(FLAGS)
	gcc -o disk.o disk.c -c $(FLAGS)
	gcc -o program.o program.c -c $(FLAGS)
	gcc -o main.o main.c -c $(FLAGS)
	gcc -o $(EXENAME) main.o program.o disk.o page_table.o colormaster.o $(FLAGS)

clean:
	rm -f $(EXENAME) *.o *.tar.gz myvirtualdisk

run:
	./$(EXENAME) 10 6 fifo pattern2

compress:
	tar -zcvf TAREA2_fjji_ifgarces.tar.gz Makefile *.c *.h informe-t2.pdf