CC = gcc

CFLAGS = -g -std=c99

LDFLAGS = -lcmocka

.PHONY: clean all check

%.o: %.c 
	$(CC) $(CFLAGS) -c -o $@ $<

#For all compiling, building and executing
all: main DNA DNA_bin check 

#For only building and testing interface
build: DNA DNA_bin test_DNA test_DNA_bin

main: main.o gene.o
	$(CC) $(CFLAGS) -o $@ $^

gene.o: gene.h

check: test_gene test_DNA test_DNA_bin test_gene_bin

test_gene: test_gene.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_gene.o: gene.c

test_gene_bin: test_gene_bin.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_gene_bin.o: gene_bin.c

DNA : 
	python3 setup.py build
	cp build/lib*/*.so .

DNA_bin : 
	python3 setup_bin.py build -b build_bin
	cp build_bin/lib*/*.so .

test_DNA : 
	python3 -m pytest -s

test_DNA_bin : 
	python3 -m pytest -s

clean :
	@rm -rf __pycache__ build build_bin output .pytest_cache *.so
	@rm -f *.o main test_gene test_gene_bin

