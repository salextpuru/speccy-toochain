
EXE=scr2spr scr2sprn

all:$(EXE)

scr2spr:
	$(CC) -D WITH_ATTRS=1 scr2spr.c
	$(LD) -o scr2spr scr2spr.o

scr2sprn:
	$(CC) -D WITH_ATTRS=0 scr2spr.c
	$(LD) -o scr2sprn scr2spr.o

clean:
	rm -f *.o $(EXE)

install:
	@for i in  $(EXE) ; do \
	echo "Installing: "$$i ; \
	cp -f $$i $(BINDIR) ; \
	done
