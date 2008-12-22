##Config
CC       = gcc
CFLAGS   = -Wall -Os
LDFLAGS  = -Wall -Os
ifdef DEBUG
CFLAGS  += -g -DDEBUG
LDFLAGS += -g -DDEBUG
endif
DOXYGEN  = doxygen
VALGRIND = valgrind -v --leak-check=full

##Files
HEADER = flog.h flog_stdio.h flog_file.h
SRC = test.c flog.c flog_stdio.c flog_file.c
OBJ = $(SRC:.c=.o)

##Rules
.PHONY : all clean distclean valgrind_test

all: test doxygen

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(OBJ) $(HEADER)
	$(CC) $(LDFLAGS) $^ -o $@

doxygen: Doxyfile $(SRC) $(HEADER)
	$(DOXYGEN)

valgrind_test: test
	$(VALGRIND) ./$<

clean:
	$(RM) $(OBJ) test
	
distclean: clean
	$(RM) -r doxygen
