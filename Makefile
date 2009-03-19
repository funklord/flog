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
HEADER = config.h flog_msg_id.h flog.h flog_string.h flog_output_stdio.h flog_output_file.h
SRC = flog_msg_id.c flog.c flog_string.c flog_output_stdio.c flog_output_file.c test.c
OBJ = $(SRC:.c=.o)

##Rules
.PHONY : all clean distclean valgrind_test

all: test

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(OBJ) $(HEADER)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

doxygen: Doxyfile $(SRC) $(HEADER)
	$(DOXYGEN)

valgrind_test: test
	$(VALGRIND) ./$<

clean:
	$(RM) $(OBJ) test

distclean: clean
	$(RM) -r doxygen
