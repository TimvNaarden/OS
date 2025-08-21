# Source directories
# Assume SRC_DIRS is something like: src lib
SRC_DIRS := src 

# Function to recursively find all .c files
define find-c-sources
$(wildcard $(addsuffix /*.c,$1)) \
$(foreach dir,$(wildcard $(addsuffix /*,$1)),$(call find-c-sources,$(dir)))
endef

C_SOURCES := $(foreach dir,$(SRC_DIRS),$(call find-c-sources,$(dir)))
C_SOURCES += GNU-EFI/lib/data.c
# Just for debugging
$(info C_SOURCES = $(C_SOURCES))

# Output dirs
OBJDIR := bin-int
OUTPUT_DIR := bin
TARGET := $(OUTPUT_DIR)/BOOTX64.EFI

# Compiler and linker
CC := clang
LD := lld-link

# Common compile flags
CFLAGS := -target x86_64-unknown-windows \
          -std=c17 \
          -Wall -Wextra -Wpedantic \
          -mno-red-zone \
          -ffreestanding \
          -fshort-wchar \
					-IGNU-EFI/inc -IGNU-EFI/inc/x86_64 -IGNU-EFI/inc/protocol -I/usr/include

# Linker flags
LDFLAGS := -target x86_64-unknown-windows \
           -nostdlib \
           -fuse-ld=$(LD) \
           -Wl,-subsystem:efi_application \
           -Wl,-entry:efi_main

# Convert .c -> .o inside bin-int/
OBJS := $(C_SOURCES:%.c=$(OBJDIR)/%.o)

# Default build
all: $(TARGET)
	dd if=/dev/zero of=bin/fat.img bs=1k count=1440
	mformat -i bin/fat.img -f 1440 ::
	mmd -i bin/fat.img ::/EFI
	mmd -i bin/fat.img ::/EFI/BOOT
	mcopy -i bin/fat.img bin/BOOTX64.EFI ::/EFI/BOOT
	mkgpt -o bin/hdimage.bin --image-size 4096 --part bin/fat.img --type system
	qemu-system-x86_64 -L OVMF_dir/ -pflash OVMF.fd -hda bin/hdimage.bin

# Link step
$(TARGET): $(OBJS)
	@mkdir -p $(OUTPUT_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

# Compile step
$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean
clean:
	rm -rf $(OBJDIR) $(OUTPUT_DIR) .cache
