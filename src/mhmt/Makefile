APP=mhmt

OBJS=mhmt-emit.o mhmt-globals.o mhmt-hash.o mhmt-lz.o mhmt-main.o mhmt-optimal.o mhmt-pack.o mhmt-parsearg.o mhmt-tb.o mhmt-depack.o

all:$(OBJS)
	$(LD) -o $(APP) $(OBJS)

%.o :%.c
	$(CC) -c $<

clean:
	rm -f $(OBJS) $(APP)

install:
	@for i in  $(APP) ; do \
	echo "Installing: "$$i ; \
	cp -f $$i $(BINDIR) ; \
	done
