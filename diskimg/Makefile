
include ../config.mk

TARGET = mcpgos.iso
CONTENT = content
KERNEL = ../kernel/mcpgos.kernel

.PHONY: all clean $(TARGET)

all: $(TARGET)

clean:
	$(RM) $(TARGET)

$(TARGET):
	$(CP) $(KERNEL) $(CONTENT)/boot
	grub-mkrescue -o $@ $(CONTENT)
