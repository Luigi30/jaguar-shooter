BINFILE = tyrian.jag

BINPATH = bin/
OBJPATH = ../obj/

OBJFILES = $(OBJPATH)tyrian.o $(OBJPATH)sprite.o $(OBJPATH)fixed.o $(OBJPATH)blit.o $(OBJPATH)bullet.o $(OBJPATH)sprites.o $(OBJPATH)mobj.o $(OBJPATH)images.o $(OBJPATH)palette.o $(OBJPATH)paldata.o
IMAGES = images/atarifont.s images/atarifont8x8.s images/shipsheet.s

UNAME := $(shell uname)
ifeq ($(findstring NT-5.1,$(VARIABLE)),CYGWIN)
    # Found
    VJAGFOLDER=/cygdrive/e/virtualjaguar/
else
    # Not found
    VJAGFOLDER=e:/virtualjaguar/
endif

DOCKER = docker run --rm -v c:/jaguar/tyrian/src:/usr/src/compile --workdir /usr/src/compile toarnold/jaguarvbcc:0.9f
CC = vc
AS = ~/vasm/vasmjagrisc_madmac
JAGINCLUDE = /opt/jagdev/targets/m68k-jaguar/include
CONVERT = tools/converter.exe --target-dir images/ 

RMAC = ./rmac.exe
RMACFLAGS = -i$(JAGINCLUDE)

.PHONY: clean

all: build

build:	$(IMAGES) $(OBJFILES)
	$(DOCKER) $(CC) -v -O0 +jaguar.cfg -lm -o $(BINPATH)$(BINFILE) $(OBJFILES)

clean:
	-rm obj/*
	-rm bin/*

run:
        #Adjust this path to your configuration.
	$(VJAGFOLDER)virtualjaguar.exe --alpine C:\jaguar\tyrian\src\bin\tyrian.jag

upload:
	-jcp -r
	sleep 2
	jcp -c $(BINPATH)$(BINFILE)

$(OBJPATH)%.o: %.c
	$(DOCKER) $(CC) +jaguar.cfg -c -c99 -o $@ $?

$(OBJPATH)%.o: %.asm
	$(DOCKER) $(CC) +jaguar.cfg -c -c99 -o $@ $?

$(OBJPATH)%.o: %.tom.s
	$(AS) $? -I$(JAGINCLUDE) -Fvobj -mgpu -o $@

$(OBJPATH)%.o: %.jerry.s
	$(AS) -L $@.lst $? -I$(JAGINCLUDE) -Fvobj -mdsp -o $@

#Images
images/atarifont.s: images/atarifont.gif
	$(CONVERT) --opt-clut --clut $? 

images/atarifont8x8.s: images/atarifont8x8.bmp
	$(CONVERT) --opt-clut --clut $? 

images/shipsheet.s: images/shipsheet.bmp
	$(CONVERT) --opt-clut --clut $? 
