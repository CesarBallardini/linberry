linberry_syncdir=@OPENSYNC_PLUGINDIR@
configdir=@OPENSYNC_CONFIGDIR@
#formatsdir=@OPENSYNC_FORMATSDIR@
opensyncheaderdir=@OPENSYNC_HEADERDIR@

INCLUDES = @TREE_BUILD_CXXFLAGS@ @PACKAGE_CFLAGS@ @LINBERRY_CFLAGS@
LIBS = @TREE_BUILD_LDFLAGS@

AM_CFLAGS = -Wall -Werror
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS =
if WITH_GCCVISIBILITY
AM_CFLAGS += -D__LINBERRY_HAVE_GCCVISIBILITY__ -fvisibility=hidden
#AM_CXXFLAGS += -D__LINBERRY_HAVE_GCCVISIBILITY__ -fvisibility=hidden -fvisibility-inlines-hidden
AM_CXXFLAGS += -D__LINBERRY_HAVE_GCCVISIBILITY__ -fvisibility=hidden
#AM_LDFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden
AM_LDFLAGS += -fvisibility=hidden
endif

EXTRA_DIST = linberry_sync.h

dist_config_DATA = linberry-sync

opensyncheader_HEADER = linberry_sync.h

linberry_sync_LTLIBRARIES = linberry_sync.la

linberry_sync_la_SOURCES = \
	linberry_sync.cc linberry_sync.h \
	environment.cc environment.h \
	vbase.cc vbase.h \
	vevent.cc vevent.h \
	vcard.cc vcard.h \
	idmap.cc idmap.h \
	vformat.c vformat.h \
	trace.h

linberry_sync_la_LDFLAGS = -avoid-version -export-dynamic -module
linberry_sync_la_LIBADD = @PACKAGE_LIBS@ @LIBS@ @LINBERRY_LIBS@

## if you plan to install format libraries: ##
#formats_LTLIBRARIES = format.la
#
#format_la_SOURCES = format.c
#format_la_LDFLAGS = -avoid-version -export-dynamic -module
#format_la_LIBADD = @PACKAGE_LIBS@ @LIBS@

