TARGET = qtlifx
CONFIG += debug c++17 shared
TEMPLATE = lib
QT -= gui
QT += core network

INCLUDEPATH += public

SOURCES = src/lifxbulb.cpp \
          src/lifxgroup.cpp \
	  src/lifxmanager.cpp \
	  src/lifxpacket.cpp \
	  src/lifxproduct.cpp \
	  src/lifxprotocol.cpp

HEADERS = public/defines.h \ 
          public/lifxbulb.h \
	  public/lifxgroup.h \
	  public/lifxmanager.h \
	  public/lifxpacket.h \
	  public/lifxproduct.h \
	  public/lifxprotocol.h



