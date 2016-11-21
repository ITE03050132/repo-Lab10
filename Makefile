Lab10: example.c
	gcc -o example example.c -lpthread
thread.o: example.c
	gcc -c example.c
clean:
	rm *.o Lab10
