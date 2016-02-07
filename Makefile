.PHONY: clean all debug run
.DEFAULT_GOAL := all

BIN_NAME=tetris

CC=gcc

IDIR=inc
LDIR=lib
ODIR=obj
BDIR=bin

CFLAGS=-Werror -Wall -pedantic -g
INCS=-I$(IDIR)
LIBS=-lpthread

DEPS=
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,$(ODIR)/%.o,$(filter-out $(patsubst %,%.c,$(BIN_NAME)), $(SRCS)))
BINS=$(patsubst %,$(BDIR)/%,$(BIN_NAME))

$(BDIR) $(ODIR):
	@echo "MKDIR $@"
	@mkdir -p $@

$(ODIR)/%.o: %.c $(DEPS) | $(ODIR)
	@echo "GCC   $^ -> $@"
	@$(CC) -c -o $@ $< $(CFLAGS) $(INCS)

$(BINS): $(BDIR)/%: $(ODIR)/%.o $(OBJS) | $(BDIR)
	@echo "GCC   $@"
	@gcc -o $@ $^ $(CFLAGS) $(LIBS)

all: $(BINS)

debug: all
	-@gdb ./bin/tetris -ex "r" 2>/dev/null; true

run: all
	-@./bin/tetris 2>/dev/null; true

clean:
	@echo "CLEAN $(ODIR) $(BDIR)"
	@rm -rf $(ODIR) $(BDIR)
