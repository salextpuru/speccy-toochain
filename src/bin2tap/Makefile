
EXE=bin2tap

all:$(EXE)
	

bin2tap:
	$(CC) bin2tap.c -o bin2tap.o
	$(LD) bin2tap.o -o bin2tap


clean:
	rm -f *.o $(EXE)

install:
	@for i in  $(EXE) ; do \
	echo "Installing: "$$i ; \
	cp -f $$i $(BINDIR) ; \
	done
