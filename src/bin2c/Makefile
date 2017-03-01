
EXE=bin2c

all:$(EXE)
	

bin2c:
	$(CC) bin2c.c
	$(LD) -o bin2c bin2c.o


clean:
	rm -f *.o $(EXE)

install:
	@for i in  $(EXE) ; do \
	echo "Installing: "$$i ; \
	cp -f $$i $(BINDIR) ; \
	done
