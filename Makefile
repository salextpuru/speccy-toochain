VERSION=3.5
TOOLS=
# -------------------------------------------------
# Tools List (to compile tool - uncommrnt it)
# -------------------------------------------------
TOOLS+=megalz
TOOLS+=mktrd
TOOLS+=hex2bin
TOOLS+=dz80
TOOLS+=bin2c
TOOLS+=zmakebas
TOOLS+=zxsend
TOOLS+=snatzx
TOOLS+=unsorted0
TOOLS+=zmac
TOOLS+=bin2tap
TOOLS+=scr2spr
TOOLS+=zxscredit
TOOLS+=alasm2txt
TOOLS+=z80asm
TOOLS+=spgbld
TOOLS+=mhmt

# -------------------------------------------------
export TOOLS
# export CC=gcc -m32 -O3 -c
# export CXX=g++ -m32 -O3 -c
# export LD=g++ -m32

export CC=gcc -O3 -c
export CXX=g++ -O3 -c
export LD=g++

export TOPDIR=$(shell pwd)
export DESTDIR=$(TOPDIR)/install
export BINDIR=$(DESTDIR)/bin
export SRCDIR=$(TOPDIR)/src

# Список
export TOOLDIR=$(SDKDIR)/tools.src

# Всё в том порядке, что написано
all: prepare
	@for i in  $(TOOLS) ; do \
	echo $$i ; \
	make -C $(SRCDIR)/$$i ; \
	done

# Подготовочная хрень
prepare:
	@echo "- Утилиты для спектрум. -"
	@echo "Корень: $(TOPDIR)"
	@echo "Дерево инсталляции $(DESTDIR)"


clean:
	@for i in  $(TOOLS) ; do \
	echo $$i ; \
	make -C $(SRCDIR)/$$i clean ; \
	done
	rm -rf $(DESTDIR)/*
	rm -f version

install: version
	mkdir -p $(BINDIR)
	@for i in  $(TOOLS) ; do \
	echo $$i ; \
	make -C $(SRCDIR)/$$i install ; \
	done
	chmod -R u+rw $(DESTDIR)/*

version:
	echo "speccy-toolchain-$(VERSION)" > version
