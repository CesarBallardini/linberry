
#DEFAULT_INCLUDES =
INCLUDES = $(LIBUSB_CFLAGS)
AM_CXXFLAGS = -ansi -Wall -g

noinst_PROGRAMS = addcontact addcalendar

addcontact_SOURCES = addcontact.cc
addcontact_LDADD = ../src/liblinberry.la $(LIBUSB_LIBS) -lpthread

addcalendar_SOURCES = addcalendar.cc
addcalendar_LDADD = ../src/liblinberry.la $(LIBUSB_LIBS) -lpthread

