all: compile exe

compile:
	gcc -o main main.c

exe:
	./all.sh

clean:
	rm -f main *.bin*
