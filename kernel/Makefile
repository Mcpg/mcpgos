
include ../config.mk

TARGET = mcpgos.kernel

SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin

ifeq ($(DEBUG),true)
	CFLAGS := $(CFLAGS) -g -DDEBUG
	ASFLAGS := $(ASFLAGS) -F dwarf -g -DDEBUG
	BIN_DIR := $(BIN_DIR)/debug
else
	BIN_DIR := $(BIN_DIR)/ndebug
endif

CFLAGS := $(CFLAGS) -I$(INCLUDE_DIR) -ffreestanding -nostdlib
CFLAGS := $(CFLAGS) -I../libc/include
ASFLAGS := $(ASFLAGS) -f elf -I $(INCLUDE_DIR)/ -g

LINKER_SCRIPT = $(SRC_DIR)/mcpgos.ld

SOURCES = $(shell find $(SRC_DIR)/ -type f -name '*.c')
SOURCES := $(SOURCES) $(shell find $(SRC_DIR)/ -type f -name '*.asm')
 
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(SOURCES))
OBJECTS := $(patsubst $(SRC_DIR)/%.asm,$(BIN_DIR)/%.o,$(OBJECTS))
OBJECTS := $(OBJECTS) ../libc/klibc.a

.PHONY: all clean

all: $(TARGET)

clean:
	$(RM) $(TARGET) $(OBJECTS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.asm
	@$(MKDIR) $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(LINKER_SCRIPT) $(OBJECTS)
	@$(MKDIR) $(dir $@)
	$(CC) -nostdlib -lgcc -o $@ -T $^
	@echo " ::: Detecting Multiboot2 compatibility..."
	grub-file --is-x86-multiboot2 $@
