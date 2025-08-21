#include "efi.h"
#include <efi.h>
#include <efilib.h>

#include "../util/mem.h"
#include "../util/util.h"

#include "../games/2048.h"
#include "../games/tic_tac_toe.h"
#include "eficon.h"

char *helpMenu = "List of Commands:\r\n\
--------------------------------------------------\r\n\
exit: Shutdown The Pc\r\n\
clear: CLear the Screen\r\n\
2048: Play 2048\r\n\
ttt: Play Tic Tac Toe against the computer\r\n\r\n";
EFI_SYSTEM_TABLE *g_SystemTable;

CHAR16 key_buffer[256];

int game = 0;
Board *b;

void toggle_game() {
  if (game) {
    mem_free(b);
    game = 0;

  } else if (!game) {
    b = (Board *)mem_alloc(sizeof(Board));
    game = 1;
    initBoard(b);
    printBoard(b);
  }
}

int TTT;
TTT_Board *bT;

void toggle_tic_tac_toe() {
  if (TTT) {
    mem_free(bT);
    TTT = 0;
    print_string("> ");
  } else if (!TTT) {
    bT = (TTT_Board *)mem_alloc(sizeof(TTT_Board));
    TTT = 1;
    initTTT_Board(bT);
    printTTT_Board(bT);
  }
}

void execute_command(char *input) {
  if (compare_string(input, "exit") == 0) {
    print_string("Shutting Down Bye!\n");
    g_SystemTable->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS,
                                                0, NULL);
  } else if (compare_string(input, "clear") == 0) {
    clear_screen();
    print_string("> ");
  } else if (compare_string(input, "2048") == 0) {
    toggle_game();
  } else if (compare_string(input, "ttt") == 0) {
    toggle_tic_tac_toe();
  } else if (compare_string(input, "help") == 0) {
    print_string(helpMenu);
    print_string("> ");
  } else {
    print_string("Unknown command: ");
    print_string(input);
    print_string("\r\n> ");
  }
}
static void handleKeyPress(EFI_INPUT_KEY inputKey) {
  uint16_t scancode = inputKey.ScanCode;
  if (scancode > 0xFFFF)
    return;
  if (game) {
    if (inputKey.UnicodeChar == 'w' || scancode == 0x01) {
      if (moveUp(b)) {
        toggle_game();
        print_string("\r\n> ");
      }
    } else if (inputKey.UnicodeChar == 's' || scancode == 0x02) {
      if (moveDown(b)) {
        toggle_game();
        print_string("\r\n> ");
      }
    } else if (inputKey.UnicodeChar == 'd' || scancode == 0x03) {
      if (moveRight(b)) {
        toggle_game();
        print_string("\r\n> ");
      }
    } else if (inputKey.UnicodeChar == 'a' || scancode == 0x04) {
      if (moveLeft(b)) {
        toggle_game();
        print_string("\r\n> ");
      }
    } else if (inputKey.UnicodeChar == u'u') {
      toggle_game();
      print_string("\r\n> ");
    }
    return;
  }
  if (TTT) {
    if (inString(inputKey.UnicodeChar, "0123456789")) {
      int result = moveTTT(bT, inputKey.UnicodeChar);
      if (result == 1) {
        print_string("Box is alreay occupied!\r\n");
      } else if (result == 'X') {
        print_string("You have won!\r\n");
        toggle_tic_tac_toe();
      } else if (result == 'O') {
        print_string("The computer has won!\r\n");
        toggle_tic_tac_toe();
      }

      else {
        clear_screen();
        printTTT_Board(bT);
      }
    }
    return;
  }

  if (scancode == 0x008) {
    if (backspace(key_buffer)) {
      g_SystemTable->ConOut->OutputString(g_SystemTable->ConOut, u"U+0008");
    }
  } else if (inputKey.UnicodeChar == CHAR_CARRIAGE_RETURN &&
             scancode == SCAN_NULL) {
    print_nl();
    execute_command(key_buffer);
    key_buffer[0] = u'\0';

  } else {
    char letter = inputKey.UnicodeChar;
    append(key_buffer, letter);
    char str[2] = {letter, u'\0'};
    print_string(str);
  }
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle,
                           EFI_SYSTEM_TABLE *SystemTable) {
  (void)ImageHandle; // Prevent compiler warning
  g_SystemTable = SystemTable;
  SystemTable->ConOut->SetAttribute(SystemTable->ConOut,
                                    EFI_TEXT_ATTR(EFI_BLACK, EFI_WHITE));

  // Clear screen to bg color
  SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
  init_memory();
  // SystemTable->ConOut->OutputString(SystemTable->ConOut,u"Press esc to
  // shutdown...\r\n");
  EFI_INPUT_KEY key;

  do {
    int status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &key);
    if (status == EFI_SUCCESS) {
      handleKeyPress(key);
      /*
      SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Key Pressed: ");
      SystemTable->ConOut->OutputString(SystemTable->ConOut, &key.UnicodeChar);
      SystemTable->ConOut->OutputString(SystemTable->ConOut, u"\r\n");*/
    }
  } while (key.ScanCode != 0x17);
  // Exit loop if ESC key is pressed}
  // Shutdown, does not return
  SystemTable->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0,
                                            NULL);

  // Should never get here
  return EFI_SUCCESS;
}
