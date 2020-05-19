IDIR =include
CC=cc
CFLAGS=-I$(IDIR)

ODIR=src

LIBS=-lm

_DEPS = parser.h rule.h
DEPS = $(patsubst %,$(ODIR)/%,$(_DEPS))

_OBJ = parser.c rule.c bake.c
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 