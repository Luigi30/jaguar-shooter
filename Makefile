BINFILE = tyrian.jag

SRCPATH = .
BINPATH = ../bin/
OBJPATH = ../obj/
TOOLPATH= ../tools/

OBJFILES = $(OBJPATH)tyrian.o $(OBJPATH)utils/list.o $(OBJPATH)utils/list_functions.o $(OBJPATH)gfx/background.o $(OBJPATH)gfx/sprite.o $(OBJPATH)fixed.o $(OBJPATH)gfx/blit.o $(OBJPATH)bullet.o $(OBJPATH)gfx/sprites.o $(OBJPATH)mobj.o $(OBJPATH)images.o $(OBJPATH)gfx/palette.o $(OBJPATH)paldata.o
IMAGES = images/atarifont.s images/atarifont8x8.s images/shipsheet.s

UNAME := $(shell uname -a)
ROMPATH = C:\jaguar\tyrian\bin\tyrian.jag

ifeq ($(findstring Microsoft,$(UNAME)),Microsoft)
    VJAGFOLDER=/mnt/e/virtualjaguar/
    ROMPATH=/mnt/c/jaguar/tyrian/bin/tyrian.jag
else ifeq ($(findstring NT-5.1,$(VARIABLE)),CYGWIN)
    VJAGFOLDER=/cygdrive/e/virtualjaguar/
else
    # Not found
    VJAGFOLDER=e:/virtualjaguar/
endif

DOCKER = docker.exe run --rm -v c:/jaguar/tyrian/:/usr/src/compile --workdir /usr/src/compile/src toarnold/jaguarvbcc:0.9f
CC = vc
AS = ~/vasm/vasmjagrisc_madmac
JAGINCLUDE = /opt/jagdev/targets/m68k-jaguar/include
CONVERT = $(TOOLPATH)/converter.exe --target-dir images/ 

GNU_CC = m68k-elf-gcc-7.2.0
CFLAGS = -O0 -MP -MD -std=gnu11 -nostartfiles -I${VBCC}/targets/m68k-jaguar/include

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
	$(VJAGFOLDER)virtualjaguar.exe --alpine $(ROMPATH)

upload:
	-jcp -r
	sleep 2
	jcp -c $(BINPATH)$(BINFILE)

$(OBJPATH)%.o: %.c %.h
	@mkdir -p $(@D)
	$(DOCKER) $(CC) -I$(SRCPATH) +jaguar.cfg -c -c99 -o $@ $?

$(OBJPATH)%.o: %.asm
	$(DOCKER) $(CC) +jaguar.cfg -c -c99 -o $@ $?

$(OBJPATH)%.o: %.tom.s
	$(AS) -L $@.lst $? -I$(JAGINCLUDE) -I$(SRCPATH) -Fvobj -mgpu -o $@

$(OBJPATH)%.o: %.jerry.s
	$(AS) -L $@.lst $? -I$(JAGINCLUDE) -I$(SRCPATH) -Fvobj -mdsp -o $@

#Images
images/atarifont.s: images/atarifont.gif
	$(CONVERT) --opt-clut --clut $? 

images/atarifont8x8.s: images/atarifont8x8.bmp
	$(CONVERT) --opt-clut --clut $? 

images/shipsheet.s: images/shipsheet.bmp
	$(CONVERT) --opt-clut --clut $? 
