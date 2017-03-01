
PREFIX=$(DESTDIR)

BINDIR=$(PREFIX)/bin
MANDIR=$(PREFIX)/man/man1

YACC = bison -y

MV = mv

ZIP = zip

CFLAGS = -O -Wall

all:	zmac

zmac:	zmac.c mio.c getoptn.c
	$(CC) $(CFLAGS) zmac.c
	$(CC) $(CFLAGS) mio.c
	$(CC) $(CFLAGS) getoptn.c
	$(LD) $(CFLAGS) -o zmac *.o

zmac.c: zmac.y
	$(YACC) zmac.y
	$(MV) y.tab.c zmac.c

install:
	install -m 511 zmac $(BINDIR)
	install -m 444 zmac.1 $(MANDIR)

zip:
	$(RM) zmac.zip
	man -l -7 zmac.1 | sed 's/.//g' >zmac.doc
	$(ZIP) zmac.zip zmac.y zmac.c mio.c mio.h getoptn.c getoptn.h zmac.1 \
	  zmac.doc Makefile README NEWS COPYRIGHT ChangeLog MAXAM RISCOS \
	  file_id.diz

clean:
	$(RM) *.o y.tab.c zmac.c *~ zmac
