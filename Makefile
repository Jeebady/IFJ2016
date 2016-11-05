# Project: IFJ16 language interpret
# File: Makefile
# Description: Lexical analyser makefile
# Author: xandri03
# Last edit: 15/10/16

CC=gcc
CFLAGS= -std=c99 -Wall -Wextra -pedantic
LDFLAGS=
SOURCES= scanner.c parser.c test.c
EXE=exe

TEST1IN=./test/t1.ifj
TEST1OUT=t1.out
REF1OUT=./test/ref1.out

TEST2IN=./test/t2.ifj
TEST2OUT=t2.out
REF2OUT=./test/ref2.out

TEST3IN=./test/t3.ifj
TEST3OUT=t3.out
REF3OUT=./test/ref3.out

all: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXE)  $(LDFLAGS)

clean:
	rm $(EXE)  test.java *.class *.out

run: $(EXE)
	./$(EXE)

test1: $(EXE) $(TEST1IN) $(REF1OUT)
	./$(EXE) <$(TEST1IN) >$(TEST1OUT);
test2: $(EXE) $(TEST2IN) $(REF2OUT)
	./$(EXE) <$(TEST2IN) >$(TEST2OUT);
test3: $(EXE) $(TEST3IN) $(REF3OUT)
	./$(EXE) <$(TEST3IN) >$(TEST3OUT);
test_all:
	make test1; make test2; make test3

java:
	cat ifj16.java test.ifj > test.java; javac test.java; java ifj16
