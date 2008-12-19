##Config
CC      = gcc
ifdef DEBUG
CFLAGS  = -Wall -Os
LDFLAGS = -Wall -Os
else
CFLAGS  = -Wall -Os -g -DDEBUG
LDFLAGS = -Wall -Os -g
endif

DOXYGEN       = doxygen
DOXYGEN_OPTS  =

VALGRIND      = valgrind
VALGRIND_OPTS = -v --leak-check=full

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
	$(DOXYGEN) $(DOXYGEN_OPTS)

valgrind_test: test
	$(VALGRIND) $(VALGRIND_OPTS) ./$<

clean:
	$(RM) $(OBJ) test
	
distclean: clean
	$(RM) -r doxygen
