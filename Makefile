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

LCDEFS =	-DNU_DEBUG -DF3DEX_GBI_2
LDIRT  =	$(APP) $(TARGETS)
LCINCS =	-I. -I$(NUSYSINCDIR) -I$(ROOT)/usr/include/PR -I/usr/include/n64/nustd
LCOPTS =	-G 0
LDFLAGS = $(MKDEPOPT) -L$(LIB) -L$(NUSYSLIBDIR) -lnusys_d -lultra_d -L$(N64_LIBGCCDIR) -lgcc  -lnustd

OPTIMIZER =	-g

APP =		sandbox.out

TARGETS =	sandbox.n64

HFILES =	main.h renderer.h game.h input.h vec3.h vec2.h quat.h matrix.h entity.h transform.h camera.h

CODEFILES   = 	main.c  renderer.c input.c game.c cube.c vec3.c quat.c matrix.c entity.c transform.c camera.c

CODEOBJECTS =	$(CODEFILES:.c=.o)  $(NUSYSLIBDIR)/nusys.o

DATAFILES   =	cube.c

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
