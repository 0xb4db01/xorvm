CC=gcc

all:
	$(CC) src/tests.c -o build/release/tests

	./build/release/tests

clean:
	rm -rf src/*.o
	rm -rf build/release/tests
