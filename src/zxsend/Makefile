# makefile for zxsend

# for Linux:
DEFAULT_OUTPUT=\"/dev/dsp\"

PREFIX=$(DESTDIR)
BINDIR=$(PREFIX)/bin

CFLAGS=-O -DDEFAULT_OUTPUT=$(DEFAULT_OUTPUT)


OBJS=zxsend.o getoptn.o

zxsend: $(OBJS)
	$(LD) $(CFLAGS) -o zxsend $(OBJS)

install: zxsend
	install -m 511 zxsend $(BINDIR)

clean:
	$(RM) *~ *.o zxsend
