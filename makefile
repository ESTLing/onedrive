client : main.o	http.o	filedata.o operator.o
	gcc main.o http.o filedata.o operator.o -o client -l ssl
main.o : main.c operator.h 
	gcc -Wall -g -c main.c
http.o:	 http.c
	gcc -Wall -g -c http.c
filedata.o:  filedata.c http.h
	gcc -Wall -g -c filedata.c
operator.o:  operator.c	filedata.c
	gcc -Wall -g -c operator.c
clean:
	rm main.o http.o filedata.o operator.o
