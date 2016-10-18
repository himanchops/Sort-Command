mysort: mysort.o
	cc mysort.o -o mysort
mysort.o: mysort.c
	cc -c mysort.c
