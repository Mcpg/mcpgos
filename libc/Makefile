
include ../config.mk

BUILD_KLIBC ?= true

SRC_DIR = src
INCLUDE_DIR = include

ifeq ($(BUILD_KLIBC),true)
    CFLAGS := $(CFLAGS) -DKERNEL
    BIN_DIR = bin/kernel
    TARGET = klibc.a
else
	BIN_DIR = bin/user
    TARGET = libc.a
endif

ifeq ($(DEBUG),true)
	CFLAGS := $(CFLAGS) -g -DDEBUG
	ASFLAGS := $(ASFLAGS) -F dwarf -g -DDEBUG
	BIN_DIR := $(BIN_DIR)/debug
else
	BIN_DIR := $(BIN_DIR)/ndebug
endif

CFLAGS := $(CFLAGS) -I$(INCLUDE_DIR) -ffreestanding -nostdlib
ASFLAGS := $(ASFLAGS) -f elf -I $(INCLUDE_DIR)/
ARFLAGS := rcs

SOURCES = $(shell find $(SRC_DIR)/ -type f -name '*.c')
SOURCES := $(SOURCES) $(shell find $(SRC_DIR)/ -type f -name '*.asm')
 
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(SOURCES))
OBJECTS := $(patsubst $(SRC_DIR)/%.asm,$(BIN_DIR)/%.o,$(OBJECTS))

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

$(TARGET): $(OBJECTS)
	@$(MKDIR) $(dir $@)
	$(AR) $(ARFLAGS) $@ $^
