##Config
CC       = gcc
CFLAGS   = -Wall -Os
LDFLAGS  = -Wall -Os
ifdef DEBUG
CFLAGS  += -g -DDEBUG
LDFLAGS += -g -DDEBUG
endif
LIB      = libflog.a
DOXYGEN  = doxygen
VALGRIND = valgrind -v --leak-check=full

##Files
HEADER = config.h flog_msg_id.h flog.h flog_string.h flog_output_stdio.h flog_output_file.h
SRC = flog_msg_id.c flog.c flog_string.c flog_output_stdio.c flog_output_file.c
OBJ = $(SRC:.c=.o)

##Rules
.PHONY : all lib clean distclean valgrind_test

all: lib

lib: $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(OBJ) $(HEADER)
	$(AR) r $(LIB) $(OBJ)

test: $(LIB) $(HEADER) test.o
	$(CC) $(LDFLAGS) test.o $(LIB) -o $@

doxygen: Doxyfile $(SRC) $(HEADER)
	$(DOXYGEN)

valgrind_test: test
	$(VALGRIND) ./$<

clean:
	$(RM) $(OBJ) $(LIB) test

distclean: clean
	$(RM) -r doxygen
	$(RM) *.log
