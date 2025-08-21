# $@ = target file
# $< = first dependency
# $^ = all dependencies

# detect all .o files based on their .c source
C_SOURCES = $(wildcard efi/*.c util/*.c games/*.c)

# Output directory
OUTPUT_DIR := bin
TARGET := $(OUTPUT_DIR)/BOOTX64.EFI

# Compiler and flags
CC := clang
LD := lld-link
CC_FLAGS := -target x86_64-unknown-windows \
            -fuse-ld=$(LD) \
            -Wl,-subsystem:efi_application \
            -Wl,-entry:efi_main \
            -std=c17 \
            -Wall \
            -Wextra \
            -Wpedantic \
            -mno-red-zone \
            -ffreestanding \
            -nostdlib \
						-I/usr/include \
						-v
# Linking rule
$(TARGET): $(C_SOURCES) 
	$(CC) $(CC_FLAGS) $^ -o $@

all:
	mkdir -p bin
	mkdir -p bin-int
	$(TARGET)

clean:
	rm bin/BOOTX64.EFI
	rm bin/test.hdd
	rm bin/DSKIMG.INF
