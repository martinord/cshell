shell: shell.c list.h list.o searchList.h searchList.o listproc.h listproc.o translations.h translations.o
	gcc -Wall -g -o shell shell.c list.o searchList.o listproc.o translations.o
	
list.o: list.c list.h
	gcc -g -c -Wall list.c

searchList: searchList.c searchList.h
	gcc -g -c -Wall searchList.c

listproc.o: listproc.c listproc.h translations.h translations.o
	gcc -g -c -Wall listproc.c 

translations.o: translations.c translations.h
	gcc -g -c -Wall translations.c

remove:
	rm shell *.o