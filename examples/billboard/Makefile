# 	NuSYSTEM samples nu2 Makefile
#       Copyright (C) 1997-1999, NINTENDO Co,Ltd.

include $(ROOT)/usr/include/make/PRdefs

# The directory which has the include file and library of NuSYSTEM
#
NUSYSINCDIR  = /usr/include/n64/nusys
NUSYSLIBDIR  = /usr/lib/n64/nusys

LIB = $(ROOT)/usr/lib
LPR = $(LIB)/PR
INC = $(ROOT)/usr/include

LCDEFS =	-DNU_DEBUG -DF3DEX_GBI_2 -DPLATFORM_N64
LDIRT  =	$(APP) $(TARGETS)
LCINCS =	-I../../include -I. -I../../assets/build_n64 -I$(NUSYSINCDIR) -I$(ROOT)/usr/include/PR -I/usr/include/n64/nustd
LCOPTS =	-G 0 -Werror
LDFLAGS = $(MKDEPOPT) -L$(LIB) -L$(NUSYSLIBDIR) -lnusys_d -lultra_d -L$(N64_LIBGCCDIR) -lgcc  -lnustd

OPTIMIZER =	-g

APP =		billboard_example.out

TARGETS =	billboard_example.n64

HFILES =	$(wildcard ../../include/framework64/*.h) $(wildcard ./*.h)

CODEFILES   =	$(wildcard ../../src/n64/*.c) $(wildcard ./*.c)

CODEOBJECTS =	$(CODEFILES:.c=.o)  $(NUSYSLIBDIR)/nusys.o

DATAFILES   =

DATAOBJECTS =	$(DATAFILES:.c=.o)

CODESEGMENT =	codesegment.o

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)


default:        $(TARGETS)

include $(COMMONRULES)

$(CODESEGMENT):	$(CODEOBJECTS) Makefile
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

$(TARGETS):	$(OBJECTS)
		$(MAKEROM) spec -I$(NUSYSINCDIR) -r $(TARGETS) -s 10 -e $(APP)
		makemask $(TARGETS)

clean:
	rm -f  ../../src/n64/*.o ./*.o ./*.out *.n64
