
CC=gcc
CFLAGS=
CLFLAGS=-lm
SRCCHRRONO=../src/uc_chrono
SRCDAYLEN=../src/lib_daylength

APP=unit-tests

all:$(APP)

unit-tests: unit-tests.c
	$(CC) $(CFLAGS) -o $(APP) unit-tests.c $(SRCCHRRONO)/uc_chrono.c $(SRCDAYLEN)/lib_daylength.c $(CLFLAGS)
	./$(APP)


#.PHONY: clean
#clean:
#	rm $(APP)


