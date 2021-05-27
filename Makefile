IDIR=.
CC=clang++
CFLAGS=-I$(IDIR)

SDIR=.
ODIR=obj

LIBS=

_DEPS = lexer.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = kaleidoscope.o lexer.o parser.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

kaleidoscope: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ kaleidoscope
