#pragma once

#include "../efi/efi.h"
#include "../util/mem.h"
#include <stdbool.h>
#include <stdint.h>

#define low_16(address) (uint16_t)((address) & 0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)

int string_length(char s[]);

void reverse(char s[]);

void int_to_string(int n, char str[]);

bool backspace(char s[]);

void append(char s[], char n);

int compare_string(char s1[], char s2[]);

void print_string(char *string);
void print_nl();
void clear_screen();
void scroll_screen(UINTN lines);
