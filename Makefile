.PHONY: clean all
.DEFAULT_GOAL := all

BIN_NAME=tetris

CC=gcc

IDIR=inc
LDIR=lib
ODIR=obj
BDIR=bin

CFLAGS=-Werror -Wall -pedantic
INCS=-I$(IDIR)
LIBS=

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

clean:
	@echo "CLEAN $(ODIR) $(BDIR)"
	@rm -rf $(ODIR) $(BDIR)
