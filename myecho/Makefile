.PHONY: all TEST clean

all: ./myecho TEST
clean:
	rm -rf myecho test out1 out2

myecho: myecho.c
	cc myecho.c -o myecho
TEST: test
	./test
test: test.c
	cc test.c -o test
