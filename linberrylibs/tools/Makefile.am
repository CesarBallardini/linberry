EXTRA_DIST = Makefile.plain

#DEFAULT_INCLUDES =
INCLUDES = $(LIBUSB_CFLAGS)
#AM_CXXFLAGS = -ansi -Wall -fno-strict-aliasing -g
AM_CXXFLAGS = -ansi -Wall -g

bin_PROGRAMS = \
	btool \
	bidentify \
	bjavaloader \
	bjvmdebug \
	bdptest \
	bjdwp \
	upldif \
	btranslate \
	bktrans \
	brecsum \
	brimtrans
if WITH_BOOST
bin_PROGRAMS += bs11nread
endif
if WITH_FUSE
bin_PROGRAMS += bfuse
endif

sbin_PROGRAMS = bcharge breset pppob

btool_SOURCES = btool.cc
btool_LDADD = ../src/liblinberry.la $(LIBUSB_LIBS) -lpthread @BOOST_LDADD@
if WITH_BOOST
btool_CXXFLAGS = -D__LINBERRY_BOOST_MODE__ -D_REENTRANT @BOOST_INC_PATH@ $(AM_CXXFLAGS)
endif
btool_LDFLAGS = @BOOST_LIB_PATH@

if WITH_BOOST
bs11nread_SOURCES = bs11nread.cc
bs11nread_LDADD = ../src/liblinberry.la $(LIBUSB_LIBS) -lpthread @BOOST_LDADD@
bs11nread_CXXFLAGS = -D_REENTRANT @BOOST_INC_PATH@ $(AM_CXXFLAGS)
bs11nread_LDFLAGS = @BOOST_LIB_PATH@
endif

bidentify_SOURCES = bidentify.cc
bidentify_LDADD = ../src/liblinberry.la $(LIBUSB_LIBS) -lpthread

bjavaloader_SOURCES = bjavaloader.cc
bjavaloader_LDADD = ../src/liblinberry.la $(LIBUSB_LIBS) -lpthread

bjvmdebug_SOURCES = bjvmdebug.cc
bjvmdebug_LDADD = ../src/liblinberry.la $(LIBUSB_LIBS) -lpthread

bdptest_SOURCES = bdptest.cc
bdptest_LDADD = ../src/liblinberrydp.la

bjdwp_SOURCES = bjdwp.cc
bjdwp_LDADD = ../src/liblinberry.la ../src/liblinberrydp.la ../src/liblinberryjdwp.la

upldif_SOURCES = upldif.cc
upldif_LDADD = ../src/liblinberry.la $(LIBUSB_LIBS) -lpthread

btranslate_SOURCES = btranslate.cc

bktrans_SOURCES = bktrans.cc
bktrans_LDADD = ../src/liblinberry.la -lpthread

bcharge_SOURCES = bcharge.cc
bcharge_LDADD = $(LIBUSB_LIBS)

breset_SOURCES = breset.cc
breset_LDADD = $(LIBUSB_LIBS)

pppob_SOURCES = pppob.cc
pppob_LDADD = ../src/liblinberry.la $(LIBUSB_LIBS) -lpthread

#brecsum_CXXFLAGS = $(OPENSSL_CFLAGS)
brecsum_SOURCES = brecsum.cc
#brecsum_LDADD = ../src/liblinberry.la $(LIBUSB_LIBS) $(OPENSSL_LIBS) -lpthread
brecsum_LDADD = ../src/liblinberry.la $(LIBUSB_LIBS) -lpthread

if WITH_FUSE
bfuse_SOURCES = bfuse.cc
bfuse_CXXFLAGS = $(FUSE_CFLAGS)
bfuse_LDADD = ../src/liblinberry.la $(FUSE_LIBS) -lpthread
endif

brimtrans_SOURCES = brimtrans.cc

