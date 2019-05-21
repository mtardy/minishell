shell: shell.o readcmd.o process_list.o
	gcc -Wextra -pedantic readcmd.o shell.o process_list.o -o shell

readcmd.o: readcmd.c readcmd.h
	gcc -Wextra -pedantic -c readcmd.c

shell.o: shell.c
	gcc -Wextra -pedantic -c shell.c

process_list.o: process_list.c process_list.h
	gcc -Wextra -pedantic -c process_list.c

test_process_list.o: test_process_list.c
	gcc -Wextra -pedantic -c test_process_list.c

test: test_process_list.o process_list.o
	gcc -Wextra -pedantic process_list.o test_process_list.o -o test_process_list

clean:
	rm -f shell test_process_list *.o
