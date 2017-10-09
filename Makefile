BINFILE = tyrian.jag

SRCPATH = .
BINPATH = ../bin/
OBJPATH = ../obj/
TOOLPATH= ../tools/

OBJFILES = $(OBJPATH)tyrian.o $(OBJPATH)utils/list.o $(OBJPATH)utils/list_functions.o $(OBJPATH)gfx/sprite.o $(OBJPATH)fixed.o $(OBJPATH)gfx/blit.o $(OBJPATH)bullet.o $(OBJPATH)gfx/sprites.o $(OBJPATH)mobj.o $(OBJPATH)images.o $(OBJPATH)gfx/palette.o $(OBJPATH)paldata.o
IMAGES = images/atarifont.s images/atarifont8x8.s images/shipsheet.s

UNAME := $(shell uname)
ifeq ($(findstring NT-5.1,$(VARIABLE)),CYGWIN)
    # Found
    VJAGFOLDER=/cygdrive/e/virtualjaguar/
else
    # Not found
    VJAGFOLDER=e:/virtualjaguar/
endif

DOCKER = docker run --rm -v c:/jaguar/tyrian/:/usr/src/compile --workdir /usr/src/compile/src toarnold/jaguarvbcc:0.9f
CC = vc
AS = ~/vasm/vasmjagrisc_madmac
JAGINCLUDE = /opt/jagdev/targets/m68k-jaguar/include
CONVERT = $(TOOLPATH)/converter.exe --target-dir images/ 

RMAC = ./rmac.exe
RMACFLAGS = -i$(JAGINCLUDE)

.PHONY: clean

all: build

build:	$(IMAGES) $(OBJFILES)
	$(DOCKER) $(CC) -v -O0 +jaguar.cfg -lm -o $(BINPATH)$(BINFILE) $(OBJFILES)

clean:
	-rm ../obj/gfx/*
	-rm ../obj/*
	-rm ../bin/*

run:
        #Adjust this path to your configuration.
	$(VJAGFOLDER)virtualjaguar.exe --alpine C:\jaguar\tyrian\bin\tyrian.jag

upload:
	-jcp -r
	sleep 2
	jcp -c $(BINPATH)$(BINFILE)

$(OBJPATH)%.o: %.c
	@mkdir -p $(@D)
	$(DOCKER) $(CC) -I$(SRCPATH) +jaguar.cfg -c -c99 -o $@ $?

$(OBJPATH)%.o: %.asm
	$(DOCKER) $(CC) +jaguar.cfg -c -c99 -o $@ $?

$(OBJPATH)%.o: %.tom.s
	$(AS) $? -I$(JAGINCLUDE) -I$(SRCPATH) -Fvobj -mgpu -o $@

$(OBJPATH)%.o: %.jerry.s
	$(AS) -L $@.lst $? -I$(JAGINCLUDE) -I$(SRCPATH) -Fvobj -mdsp -o $@

#Images
images/atarifont.s: images/atarifont.gif
	$(CONVERT) --opt-clut --clut $? 

images/atarifont8x8.s: images/atarifont8x8.bmp
	$(CONVERT) --opt-clut --clut $? 

images/shipsheet.s: images/shipsheet.bmp
	$(CONVERT) --opt-clut --clut $? 
