
EXE=hex2bin mot2bin

all:$(EXE)
	

hex2bin:
	$(CC) -c hex2bin.c
	$(LD) -o hex2bin hex2bin.o

mot2bin:
	$(CC) -c mot2bin.c
	$(LD) -o mot2bin mot2bin.o

clean:
	rm -f *.o $(EXE)

install:
	@for i in  $(EXE) ; do \
	echo "Installing: "$$i ; \
	cp -f $$i $(BINDIR) ; \
	done
