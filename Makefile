AS := arm-none-eabi-as
GCC := arm-none-eabi-gcc
CPP := arm-none-eabi-cpp
LD := arm-none-eabi-ld
OBJCOPY := arm-none-eabi-objcopy
SHA1SUM := sha1sum -c
GBAFIX := tools/gbafix/gbafix
GBAGFX := tools/gbagfx/gbagfx
SCANINC := tools/scaninc/scaninc
AIF2PCM := tools/aif2pcm/aif2pcm

# Clear the default suffixes
.SUFFIXES:
# Don't delete intermediate files
.SECONDARY:
# Delete files that weren't built properly
.DELETE_ON_ERROR:

# Secondary expansion is required for dependency variables in object rules.
.SECONDEXPANSION:


.PHONY: rom compare clean

OBJ_DIR := build/mugi

C_SUBDIR = src
ASM_SUBDIR = asm
DATA_ASM_SUBDIR = data

C_BUILDDIR = $(OBJ_DIR)/$(C_SUBDIR)
ASM_BUILDDIR = $(OBJ_DIR)/$(ASM_SUBDIR)
DATA_ASM_BUILDDIR = $(OBJ_DIR)/$(DATA_ASM_SUBDIR)

C_SRCS := $(wildcard $(C_SUBDIR)/*.c $(C_SUBDIR)/*/*.c $(C_SUBDIR)/*/*/*.c)
C_OBJS := $(patsubst $(C_SUBDIR)/%.c,$(C_BUILDDIR)/%.o,$(C_SRCS))

ASM_SRCS := $(wildcard $(ASM_SUBDIR)/*.s)
ASM_OBJS := $(patsubst $(ASM_SUBDIR)/%.s,$(ASM_BUILDDIR)/%.o,$(ASM_SRCS))

DATA_ASM_SRCS := $(wildcard $(DATA_ASM_SUBDIR)/*.s)
DATA_ASM_OBJS := $(patsubst $(DATA_ASM_SUBDIR)/%.s,$(DATA_ASM_BUILDDIR)/%.o,$(DATA_ASM_SRCS))

OBJS     := $(C_OBJS) $(ASM_OBJS) $(DATA_ASM_OBJS)
OBJS_REL := $(patsubst $(OBJ_DIR)/%,%,$(OBJS))

SUBDIRS  := $(sort $(dir $(OBJS)))

$(shell mkdir -p $(SUBDIRS))

ASFLAGS := -mcpu=arm7tdmi -mthumb-interwork

GCC_VER = $(shell $(GCC) -dumpversion)

override CFLAGS += -S -mthumb -mthumb-interwork -Ofast -mabi=apcs-gnu -mtune=arm7tdmi -march=armv4t -fno-toplevel-reorder -fno-aggressive-loop-optimizations -Wno-pointer-to-int-cast

CPPFLAGS := -iquote include

ifeq ($(DINFO),1)
CFLAGS += -g
endif

NAME := mugi
ROM := $(NAME).gba
ELF = $(ROM:.gba=.elf)
MAP = $(ROM:.gba=.map)
TITLE := K_ONBU
GAMECODE := MUGI


rom: $(ROM)

clean:
	rm -f $(ROM) $(ELF) $(MAP)
	rm -r $(OBJ_DIR)
	find . \( -iname '*.1bpp' -o -iname '*.4bpp' -o -iname '*.8bpp' -o -iname '*.gbapal' -o -iname '*.lz' -o -iname '*.striped' \) -exec rm {} +

%.s: ;
%.png: ;
%.pal: ;
%.aif: ;
%.bin: ;

%.pcm: %.aif ; $(AIF2PCM) $< $@
%.1bpp: %.png  ; $(GBAGFX) $< $@
%.4bpp: %.png  ; $(GBAGFX) $< $@
%.8bpp: %.png  ; $(GBAGFX) $< $@
%.8bpp.striped: %.png ; $(GBAGFX) $< $@
%.4bpp.striped: %.png ; $(GBAGFX) $< $@
%.gbapal: %.pal ; $(GBAGFX) $< $@
%.gbapal: %.png ; $(GBAGFX) $< $@
%.lz: % ; $(GBAGFX) $< $@
%.rl: % ; $(GBAGFX) $< $@

$(ROM): $(ELF)
	$(OBJCOPY) -O binary $< $@
	$(GBAFIX) --silent -p $@

$(OBJ_DIR)/ld_script.ld: ld_script.txt
	cd $(OBJ_DIR) && sed "s#tools/#../../tools/#g" ../../ld_script.txt > ld_script.ld

$(ELF): $(OBJS) $(OBJ_DIR)/ld_script.ld
	cd $(OBJ_DIR) && $(LD) -T ld_script.ld -Map ../../$(NAME).map -o ../../$@ $(OBJS_REL) -L /usr/lib/gcc/arm-none-eabi/$(GCC_VER)/thumb/nofp/ -L /usr/lib/arm-none-eabi/lib/thumb/nofp/ -lgcc -lm
	$(GBAFIX) -t"$(TITLE)" -c$(GAMECODE) -m69 --silent $@

$(ASM_BUILDDIR)/%.o: $(ASM_SUBDIR)/%.s
	$(AS) $(ASFLAGS) -o $@ $<

ifeq ($(NODEP),1)
$(DATA_ASM_BUILDDIR)/%.o: data_dep :=
else
$(DATA_ASM_BUILDDIR)/%.o: data_dep = $(shell $(SCANINC) -I include -I "" $(DATA_ASM_SUBDIR)/$*.s)
endif

$(DATA_ASM_BUILDDIR)/%.o: $(DATA_ASM_SUBDIR)/%.s $$(data_dep)
	$(AS) $(ASFLAGS) -o $@ $< 

$(C_BUILDDIR)/%.o: $(C_SUBDIR)/%.c
	$(GCC) $(CFLAGS) $(CPPFLAGS) $< -o $(C_BUILDDIR)/$*.s
	$(AS) $(ASFLAGS) -o $@ $(C_BUILDDIR)/$*.s