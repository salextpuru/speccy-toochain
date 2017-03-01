APP=spgbld
OBJS=func.o  main.o  msg.o

all: $(OBJS)
	$(LD) -o $(APP) $(OBJS)

%.o :%.cxx
	$(CC) -c $<

clean:
	rm -f $(OBJS) $(APP)

install:
	@for i in  $(APP) ; do \
	echo "Installing: "$$i ; \
	cp -f $$i $(BINDIR) ; \
	done
