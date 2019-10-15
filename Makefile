FLAGS = -Wall -g
EXENAME = virtmem

$(EXENAME): main.o page_table.o disk.o program.o
	gcc main.o page_table.o disk.o program.o -o $(EXENAME)

main.o: main.c
	clear;clear
	gcc $(FLAGS) -c main.c -o main.o

page_table.o: page_table.c
	gcc $(FLAGS) -c page_table.c -o page_table.o

disk.o: disk.c
	gcc $(FLAGS) -c disk.c -o disk.o

program.o: program.c
	gcc $(FLAGS) -c program.c -o program.o

clean:
	rm -f *.o $(EXENAME) *.tar.gz myvirtualdisk

run:
	./virtmem 10 6 fifo pattern2

compress:
	tar -zcvf TAREA2_fjji_ifgarces.tar.gz makefile *.c *.h informe-t2.pdf
	