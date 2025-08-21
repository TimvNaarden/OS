#include "util.h"
#include "mem.h"
#include <efi.h>
#include <uchar.h>

void uitoa_hex(unsigned int value, char *buf) {
  static const char hex[] = "0123456789ABCDEF";
  char *p = buf;

  *p++ = '0';
  *p++ = 'x';

  // convert each nibble (4 bits)
  int started = 0;
  for (int i = (sizeof(value) * 8) - 4; i >= 0; i -= 4) {
    unsigned int digit = (value >> i) & 0xF;
    if (digit != 0 || started || i == 0) {
      *p++ = hex[digit];
      started = 1;
    }
  }
  *p = '\0';
}
int string_length(char s[]) {
  int i = 0;
  while (s[i] != '\0')
    ++i;
  return i;
}

void reverse(char s[]) {
  int c, i, j;
  for (i = 0, j = string_length(s) - 1; i < j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

void int_to_string(int n, char str[]) {
  int i, sign;
  if ((sign = n) < 0)
    n = -n;
  i = 0;
  do {
    str[i++] = n % 10 + '0';
  } while ((n /= 10) > 0);
  if (sign < 0)
    str[i++] = '-';
  str[i] = '\0';

  reverse(str);
}

void append(char s[], char n) {
  int len = string_length(s);
  s[len] = n;
  s[len + 1] = '\0';
}

bool backspace(char s[]) {
  int len = string_length(s);
  if (len > 0) {
    s[len - 1] = '\0';
    return true;
  } else {
    return false;
  }
}

/* K&R
 * Returns <0 if s1<s2, 0 if s1==s2, >0 if s1>s2 */
int compare_string(char s1[], char s2[]) {
  int i;
  for (i = 0; s1[i] == s2[i]; i++) {
    if (s1[i] == '\0')
      return 0;
  }
  return s1[i] - s2[i];
}
void charToChar16(const char *input, CHAR16 *output, size_t outputSize) {
  for (size_t i = 0; i < outputSize - 1 && input[i] != '\0'; ++i) {
    output[i] = (char16_t)input[i];
  }
  output[outputSize - 1] = L'\0'; // Null-terminate the char16 string
}

void print_string(char *string) {
  // UINTN rows, cols;
  // g_SystemTable->ConOut->QueryMode(g_SystemTable->ConOut,
  // g_SystemTable->ConOut->Mode->Mode, &cols, &rows);
  int length = string_length(string) + 1;
  CHAR16 *result = (CHAR16 *)mem_alloc(length * 2);
  charToChar16(string, result, length);

  // UINTN cursorCol, cursorRow;
  // g_SystemTable->ConOut->QueryMode(g_SystemTable->ConOut,
  // g_SystemTable->ConOut->Mode->Mode, &cursorCol, &cursorRow); UINTN numLines
  // = (length + cursorCol) / cols + 1; if (numLines > rows - cursorRow) {
  //  scroll_screen(numLines - (rows - cursorRow));
  // cursorRow -= numLines - (rows - cursorRow);
  //}

  // g_SystemTable->ConOut->SetCursorPosition(g_SystemTable->ConOut, cursorCol,
  // cursorRow);
  g_SystemTable->ConOut->OutputString(g_SystemTable->ConOut, result);
  // g_SystemTable->ConOut->OutputString(g_SystemTable->ConOut, u" <- Test
  // string");
  mem_free(result);
}

void print_nl() {
  g_SystemTable->ConOut->OutputString(g_SystemTable->ConOut, L"\r\n");
}

void clear_screen() {
  g_SystemTable->ConOut->ClearScreen(g_SystemTable->ConOut);
}

int inString(char a, char *b) {
  char *c;
  for (c = b; *c != '\0'; c++) {
    if (*c == a)
      return 1;
  }
  return 0;
}
