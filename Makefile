cflags= -Wall -g -no-pie

simulate: simulate.o main.c
	gcc ${cflags} -o $@ $^

simulate.o: simulate.asm
	nasm -f elf64 -F dwarf $^ -o $@

clean:
	-rm *.o

