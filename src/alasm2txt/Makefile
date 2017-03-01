
EXE=alasm2txt alasmtxt

all:$(EXE)
	

alasm2txt:
	$(CC) alasm2txt.c
	$(LD) -o alasm2txt alasm2txt.o

alasmtxt:
	cp -f alasmtxt.src alasmtxt

clean:
	rm -f *.o $(EXE)

install:
	@for i in  $(EXE) ; do \
	echo "Installing: "$$i ; \
	cp -f $$i $(BINDIR) ; \
	done
