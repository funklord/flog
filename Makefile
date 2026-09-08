##Config
CC       = gcc
CFLAGS   = -W -Wall -Os
LDFLAGS  = -W -Wall -Os
ifdef DEBUG
CFLAGS  += -g -DDEBUG
LDFLAGS += -g -DDEBUG
endif
ifdef PROFILE
CFLAGS  += -pg
LDFLAGS += -pg
endif
LIB      = libflog.a
DOXYGEN  = doxygen
VALGRIND = valgrind -v --leak-check=full

##Files
HEADER = config.h flog_msg_id.h flog.h flog_string.h flog_output_stdio.h flog_output_file.h
SRC = flog_msg_id.c flog.c flog_string.c flog_output_stdio.c flog_output_file.c
OBJ = $(SRC:.c=.o)

##Rules
.PHONY : all lib clean distclean valgrind_test check

all: lib

lib: $(LIB)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(OBJ) $(HEADER)
	$(AR) r $(LIB) $(OBJ)

test: $(LIB) $(HEADER) test.o
	$(CC) $(LDFLAGS) test.o $(LIB) -o $@

test_pool: $(LIB) $(HEADER) test_pool.o
	$(CC) $(LDFLAGS) test_pool.o $(LIB) -o $@

#! Runs the assertions. `test` is the demonstration program and prints for a
#! person to read; this returns non-zero when something is wrong.
check: test_pool
	./test_pool

doxygen: Doxyfile $(SRC) $(HEADER)
	$(DOXYGEN)

valgrind_test: test
	$(VALGRIND) ./$<

clean:
	$(RM) $(OBJ) $(LIB) test.o test test_pool.o test_pool

distclean: clean
	$(RM) -r doxygen
	$(RM) *.log
