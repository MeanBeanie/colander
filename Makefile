CC=gcc

libcolander: colander.o
	$(CC) -shared -o libcolander.so colander.o
colander.o:
	$(CC) -c -fpic colander.c -o colander.o
clean:
	rm colander.o
