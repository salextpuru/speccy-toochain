# makefile for zspeccyutils

# this is really for tzxplay - nothing else needs much optimising.
CFLAGS+=-O2 -finline-functions

# where to install
BINDIR=$(DESTDIR)/bin

# you shouldn't need to edit past this line.


TARGETS=ldirsna infosna pokesna ppmtozx sendsna	tzxplay zxgettap zxtoppm

all:
	@for i in $(TARGETS) ; do \
	echo $$i ; \
	$(CC) $(CFLAGS) $$i.c -c ; \
	$(LD) -o $$i $$i.o ; \
	done


install:
	install -m 511 -s $(TARGETS) $(BINDIR)

clean:
	$(RM) *.o $(TARGETS) *~ *.lst
