#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

#---------------------------------------------------------------------------------
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# DATA is a list of directories containing binary files embedded using bin2o
# GRAPHICS is a list of directories containing image files to be converted with grit
# all directories are relative to this makefile
#---------------------------------------------------------------------------------
BUILD    := build
SOURCES  := source source/data source/graphics source/script source/script/commands
INCLUDES := include
DATA     := data
GRAPHICS :=
NITROSRC := nitrofilessrc nitrofilessrc/scripts nitrofilessrc/graphics

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH := -marm -mthumb-interwork -march=armv5te -mtune=arm946e-s

CFLAGS   := -g -Wall -O3\
            $(ARCH) $(INCLUDE) -DARM9
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS  := -g $(ARCH)
LDFLAGS   = -specs=ds_arm9.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS := -lnflib -lfilesystem -lfat -lNE -lnds9

# automagically add maxmod library
ifneq ($(strip $(AUDIO)),)
LIBS := -lmm9 $(LIBS)
endif

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS := $(LIBNDS) $(PORTLIBS) $(DEVKITPRO)/nitro-engine $(DEVKITPRO)/nflib

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export ARM9ELF := $(CURDIR)/arm9.elf

export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))\
                $(foreach dir,$(DATA),$(CURDIR)/$(dir))\
                $(foreach dir,$(GRAPHICS),$(CURDIR)/$(dir))\
				$(foreach dir,$(NITROSRC),$(CURDIR)/$(dir))

export DEPSDIR := $(CURDIR)/$(BUILD)

CFILES   := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES   := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
PNGFILES := $(foreach dir,$(GRAPHICS),$(notdir $(wildcard $(dir)/*.png)))
BINFILES := $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))
export NTRFILES := $(foreach dir,$(NITROSRC),$(notdir $(wildcard $(dir)/*.s)))
export GRPFILES := $(foreach dir,$(NITROSRC),$(notdir $(wildcard $(dir)/*.grp1)))
export IMGFILES := $(foreach dir,$(NITROSRC),$(notdir $(wildcard $(dir)/*.img1)))
export MAPFILES := $(foreach dir,$(NITROSRC),$(notdir $(wildcard $(dir)/*.map1)))
export PALFILES := $(foreach dir,$(NITROSRC),$(notdir $(wildcard $(dir)/*.pal1)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
  export LD := $(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
  export LD := $(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_BIN   :=	$(addsuffix .o,$(BINFILES))

export OFILES_SOURCES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export OFILES := $(PNGFILES:.png=.o) $(OFILES_BIN) $(OFILES_SOURCES)

export HFILES := $(PNGFILES:.png=.h) $(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE  := $(foreach dir,$(INCLUDES),-iquote $(CURDIR)/$(dir))\
                   $(foreach dir,$(LIBDIRS),-I$(dir)/include)\
                   -I$(CURDIR)/$(BUILD)
export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export NITROOFILES := $(NTRFILES:.s=.o)

export NITROBINFILES := $(NTRFILES:.s=.bin)

export NITROGRPFILES := $(GRPFILES:.grp1=.grp)
export NITROIMGFILES := $(IMGFILES:.img1=.img)
export NITROMAPFILES := $(MAPFILES:.map1=.map)
export NITROPALFILES := $(PALFILES:.pal1=.pal)

.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).elf


#---------------------------------------------------------------------------------
else

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
.PHONY: $(BUILD) all
all: $(NITROGRPFILES) $(NITROIMGFILES) $(NITROMAPFILES) $(NITROPALFILES) $(NITROOFILES) $(NITROBINFILES) $(ARM9ELF)

%(NITROOFILES) : $(NTRFILES) 
	@$(CC) -c -nostdlib -static $< -o $@

%.bin : %.o
	@$(OBJCOPY) -O binary $< $@
	mkdir -p nitrofiles/scripts
	if [[ $@ = scn_* ]]; then \
		cp $@ nitrofiles/scripts/$@; \
	else \
		cp $@ nitrofiles/$@; \
	fi

%.grp : %.grp1
	mkdir -p nitrofiles/graphics
	cp $< nitrofiles/graphics/$@

%.img : %.img1
	mkdir -p nitrofiles/graphics
	cp $< nitrofiles/graphics/$@

%.map : %.map1
	mkdir -p nitrofiles/graphics
	cp $< nitrofiles/graphics/$@

%.pal : %.pal1
	mkdir -p nitrofiles/graphics
	cp $< nitrofiles/graphics/$@

$(ARM9ELF) : $(OFILES)
	@echo linking $(notdir $@)
	@$(LD)  $(LDFLAGS) $(OFILES) $(LIBPATHS) $(LIBS) -o $@

#---------------------------------------------------------------------------------
%.bin.o %_bin.h : %.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
# This rule creates assembly source files using grit
# grit takes an image file and a .grit describing how the file is to be processed
# add additional rules like this for each image extension
# you use in the graphics folders
#---------------------------------------------------------------------------------
%.s %.h: %.png %.grit
#---------------------------------------------------------------------------------
	grit $< -fts -o$*

-include $(DEPSDIR)/*.d

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------