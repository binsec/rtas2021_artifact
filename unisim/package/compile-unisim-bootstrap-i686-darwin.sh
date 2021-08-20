#!/usr/bin/make -f
SHELL=/bin/bash
BUILD=i686-pc-linux-gnu
HOST=i686-apple-darwin8
CROSS_COMPILE=${HOST}-
CXX=$(CROSS_COMPILE)g++
CC=$(CROSS_COMPILE)gcc
AS=$(CROSS_COMPILE)as
AR=$(CROSS_COMPILE)ar
LD=$(CROSS_COMPILE)ld
RANLIB=$(CROSS_COMPILE)ranlib
BJAM=`which bjam`
CXXFLAGS=

INSTALL_DIR=$(PWD)/install
SOURCE_DIR=$(PWD)/source
PATCHES_DIR=$(PWD)/patches
ARCHIVE_DIR=$(PWD)/archives
NUM_PROCESSORS=`cat /proc/cpuinfo | cut -f 1 | grep vendor_id | wc -l`

all: notice $(INSTALL_DIR)/libedit $(INSTALL_DIR)/expat $(INSTALL_DIR)/zlib $(INSTALL_DIR)/gdb $(INSTALL_DIR)/SDL $(INSTALL_DIR)/libxml2 $(INSTALL_DIR)/systemc $(INSTALL_DIR)/TLM-2008-06-09 $(INSTALL_DIR)/boost
	@echo "Your built is in $(INSTALL_DIR)"

clean:
	rm -rf $(SOURCE_DIR)/*

mrproper: clean
	rm -rf $(ARCHIVE_DIR)/*
	rm -rf $(INSTALL_DIR)/*

notice:
	@echo "unisim-bootstrap-powerpc-darwin: A script to bootstrap on powerpc-apple-darwin before compiling UNISIM"
	@echo "This script is intended for Ubuntu (7.04/7.10 and maybe later) but may work with other Linux distributions"
	@echo "It will automatically download, configure, compile and install the following libraries which are needed to build UNISIM:"
	@echo "  - libedit (2.11)"
	@echo "  - zlib (1.2.3)"
	@echo "  - SDL (1.2.13)"
	@echo "  - libxml2 (2.6.31)"
	@echo "  - SystemC (2.2.0)"
	@echo "  - TLM 2.0"
	@echo "  - boost (1.34.1)"
	@echo "  - expat (2.0.1)"
	@echo "It will automatically download/configure and install the following softwares which can interact with UNISIM:"
	@echo "  - GDB (6.8) for m68hc1x, armeb and powerpc"
	@echo "The downloaded tarball/zip files will be placed in directory $(ARCHIVE_DIR)"
	@echo "They will be extracted into directory $(SOURCE_DIR)"
	@echo "Everything will be installed into directory $(INSTALL_DIR)"
	@echo "Press <ENTER> to start or <CTRL>+<C> to cancel"
	@read

# libedit

$(INSTALL_DIR)/libedit: $(SOURCE_DIR)/libedit-20080712-2.11
	cd $(SOURCE_DIR)/libedit-20080712-2.11 && \
    ./configure --prefix=$(INSTALL_DIR)/libedit --build=$(BUILD) --host=$(HOST) && \
    make -j $(NUM_PROCESSORS) && \
    make install

$(SOURCE_DIR)/libedit-20080712-2.11: $(ARCHIVE_DIR)/libedit-20080712-2.11.tar.gz
	cd $(SOURCE_DIR) && \
    tar zxvf $<

$(ARCHIVE_DIR)/libedit-20080712-2.11.tar.gz:
	cd $(ARCHIVE_DIR) && \
    wget "http://www.thrysoee.dk/editline/libedit-20080712-2.11.tar.gz"

# expat

$(INSTALL_DIR)/expat: $(SOURCE_DIR)/expat-2.0.1
	cd $(SOURCE_DIR)/expat-2.0.1 && \
    ./configure --prefix=$(INSTALL_DIR)/expat --build=$(BUILD) --host=$(HOST) && \
    make -j $(NUM_PROCESSORS) && \
    make install

$(SOURCE_DIR)/expat-2.0.1: $(ARCHIVE_DIR)/expat-2.0.1.tar.gz
	cd $(SOURCE_DIR) && \
    tar zxvf $<

$(ARCHIVE_DIR)/expat-2.0.1.tar.gz:
	cd $(ARCHIVE_DIR) && \
    wget "http://ovh.dl.sourceforge.net/expat/expat-2.0.1.tar.gz"


# gdb


$(INSTALL_DIR)/gdb: $(SOURCE_DIR)/gdb-6.8 $(INSTALL_DIR)/expat
	cd $(SOURCE_DIR)/gdb-6.8 && \
    ./configure --prefix=$(INSTALL_DIR)/gdb \
                --build=$(BUILD) --host=$(HOST) \
                --enable-targets=${HOST},m6811-elf,armeb-linux-gnu,powerpc-linux-gnu,mips-linux-gnu,sparc-linux-gnu \
                --disable-sim --disable-werror \
                --with-libexpat-prefix=$(INSTALL_DIR)/expat && \
    make -j $(NUM_PROCESSORS) && \
    make install

$(SOURCE_DIR)/gdb-6.8: $(ARCHIVE_DIR)/gdb-6.8.tar.bz2
	cd $(SOURCE_DIR) && \
    tar jxvf $<

$(ARCHIVE_DIR)/gdb-6.8.tar.bz2:
	cd $(ARCHIVE_DIR) && \
    wget "ftp://ftp.gnu.org/pub/gnu/gdb/gdb-6.8.tar.bz2"
	
# zlib

$(INSTALL_DIR)/zlib: $(SOURCE_DIR)/zlib-1.2.3
	cd $(SOURCE_DIR)/zlib-1.2.3 && \
    CC=$(CC) AR="$(AR) rc" RANLIB=$(RANLIB) ./configure --prefix=$(INSTALL_DIR)/zlib && \
    make -j $(NUM_PROCESSORS) && \
    make install

$(ARCHIVE_DIR)/zlib-1.2.3.tar.bz2:
	cd $(ARCHIVE_DIR) && \
    wget "http://www.zlib.net/zlib-1.2.3.tar.bz2" || wget "http://www.gzip.org/zlib/zlib-1.2.3.tar.bz2"

$(SOURCE_DIR)/zlib-1.2.3: $(ARCHIVE_DIR)/zlib-1.2.3.tar.bz2
	cd $(SOURCE_DIR) && \
    tar jxvf $<

# SDL

$(INSTALL_DIR)/SDL: $(SOURCE_DIR)/SDL-1.2.13
	cd $(SOURCE_DIR)/SDL-1.2.13 && \
    ./configure --host=$(HOST) --prefix=$(INSTALL_DIR)/SDL && \
    make -j $(NUM_PROCESSORS) && \
    make install

$(ARCHIVE_DIR)/SDL-1.2.13.tar.gz:
	cd $(ARCHIVE_DIR) && \
    wget http://www.libsdl.org/release/SDL-1.2.13.tar.gz

$(SOURCE_DIR)/SDL-1.2.13: $(ARCHIVE_DIR)/SDL-1.2.13.tar.gz
	cd $(SOURCE_DIR) && \
    tar zxvf $<

# libxml2

$(INSTALL_DIR)/libxml2: $(SOURCE_DIR)/libxml2-2.6.31
	cd $(SOURCE_DIR)/libxml2-2.6.31 && \
    ./configure --host=$(HOST) --prefix=$(INSTALL_DIR)/libxml2 --without-python && \
    make -j $(NUM_PROCESSORS) && \
    make install

$(ARCHIVE_DIR)/libxml2-2.6.31.tar.gz:
	cd $(ARCHIVE_DIR) && \
    wget ftp://xmlsoft.org/libxml2/libxml2-2.6.31.tar.gz

$(SOURCE_DIR)/libxml2-2.6.31: $(ARCHIVE_DIR)/libxml2-2.6.31.tar.gz
	cd $(SOURCE_DIR) && \
    tar zxvf $<

# systemc

$(INSTALL_DIR)/systemc: $(SOURCE_DIR)/systemc-2.2.0
	cd $(SOURCE_DIR)/systemc-2.2.0 && \
    mkdir -p objdir && \
    cd objdir && \
    mkdir -p $(INSTALL_DIR)/systemc && \
    ../configure --host=$(HOST) --build=$(BUILD) --prefix=$(INSTALL_DIR)/systemc AS=$(AS) AR=$(AR) && \
    make -j $(NUM_PROCESSORS) && \
    make install

$(SOURCE_DIR)/systemc-2.2.0: $(ARCHIVE_DIR)/systemc-2.2.0.tgz $(PATCHES_DIR)/patch-systemc-2.2.0
	cd $(SOURCE_DIR) && \
    tar zxvf $(ARCHIVE_DIR)/systemc-2.2.0.tgz && \
    cd $(SOURCE_DIR)/systemc-2.2.0 && \
    cat $(PATCHES_DIR)/patch-systemc-2.2.0 | patch -p1

# TLM 2.0

$(INSTALL_DIR)/TLM-2008-06-09: $(ARCHIVE_DIR)/TLM-2.0.tar.gz
	cd $(INSTALL_DIR) && \
    tar zxvf $<

# readline

$(ARCHIVE_DIR)/readline-5.2.tar.gz:
	cd $(ARCHIVE_DIR) && \
    wget "ftp://ftp.gnu.org/gnu/readline/readline-5.2.tar.gz"

$(SOURCE_DIR)/readline-5.2: $(ARCHIVE_DIR)/readline-5.2.tar.gz
	cd $(SOURCE_DIR) && \
    tar zxvf $<

$(INSTALL_DIR)/readline: $(SOURCE_DIR)/readline-5.2
	cd $(SOURCE_DIR)/readline-5.2 && \
    ./configure --prefix=$(INSTALL_DIR)/readline --host=$(HOST) --build=$(BUILD) && \
    make -j $(NUM_PROCESSORS) && \
    make install

# boost
$(ARCHIVE_DIR)/boost_1_34_1.tar.bz2:
	cd $(ARCHIVE_DIR) && \
    wget "http://ovh.dl.sourceforge.net/boost/boost_1_34_1.tar.bz2"

$(SOURCE_DIR)/boost_1_34_1: $(ARCHIVE_DIR)/boost_1_34_1.tar.bz2
	cd $(SOURCE_DIR) && \
    tar jxvf $<

$(INSTALL_DIR)/boost: $(SOURCE_DIR)/boost_1_34_1
	cd $(SOURCE_DIR)/boost_1_34_1 &&\
    rm -f g++ && ln -s `which $(CXX)` g++ &&\
    rm -f gcc && ln -s `which $(CC)` gcc &&\
    rm -f ar && ln -s `which $(AR)` ar &&\
    rm -f ranlib && ln -s `which $(RANLIB)` ranlib &&\
    rm -f as && ln -s `which $(AS)` as &&\
    rm -f ld && ln -s `which $(LD)` ld &&\
    PATH=./:${PATH} &&\
    $(SOURCE_DIR)/boost_1_34_1/configure --with-toolset=gcc --prefix=$(INSTALL_DIR)/boost --without-icu --with-libraries=graph,thread --with-bjam=$(BJAM) &&\
    make BJAM_CONFIG="-j $(NUM_PROCESSORS)" &&\
    make install &&\
    cd $(INSTALL_DIR)/boost/include && mv boost-1_34_1/boost boost && rmdir boost-1_34_1 &&\
    cd $(INSTALL_DIR)/boost/lib && cp libboost_thread-gcc40-mt-1_34_1.a libboost_thread.a && $(RANLIB) libboost_thread.a
