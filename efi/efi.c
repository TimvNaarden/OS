#include "efi.h"
#include "../util/util.h"

#include "../games/2048.h"
#include "../games/tic_tac_toe.h"
#include <efi/efiprot.h>
#include <stdint.h>

char *helpMenu = "List of Commands:\n\
--------------------------------------------------\n\
EXIT: Shutdown The Pc\n\
CLEAR: CLear the Screen\n\
2048: Play 2048\n\
TIC TAC TOE: Play Tic Tac Toe against the computer\n\n";

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
int tic_tac_toe = 0;
TTT_Board *bT;

void toggle_tic_tac_toe() {
  if (tic_tac_toe) {
    mem_free(bT);
    tic_tac_toe = 0;
    print_string("> ");
  } else if (!tic_tac_toe) {
    bT = (TTT_Board *)mem_alloc(sizeof(TTT_Board));
    tic_tac_toe = 1;
    initTTT_Board(bT);
    printTTT_Board(bT);
  }
}

void execute_command(char *input) {
  if (compare_string(input, "EXIT") == 0) {
    print_string("Shutting Down Bye!\n");
    g_SystemTable->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
  } else if (compare_string(input, "CLEAR") == 0) {
    clear_screen();
    print_string("> ");
  } else if (compare_string(input, "2048") == 0) {
    toggle_game();
  } else if (compare_string(input, "TIC TAC TOE") == 0) {
    toggle_tic_tac_toe();
  } else if (compare_string(input, "HELP") == 0) {
    print_string(helpMenu);
    print_string("> ");
  } else {
    print_string("Unknown command: ");
    print_string(input);
    print_string("\n> ");
  }
}
static void handleKeyPress(EFI_INPUT_KEY inputKey) {
  uint16_t scancode = inputKey.ScanCode;
  if (scancode > 0xFFFF)
    return;
  if (game) {
    if (scancode == 0x48 || scancode == 0x11) {
      if (moveUp(b)) {
        toggle_game();
        print_string("\n> ");
      }
    } else if (scancode == 0x50 || scancode == 0x1F) {
      if (moveDown(b)) {
        toggle_game();
        print_string("\n> ");
      }
    } else if (scancode == 0x4D || scancode == 0x20) {
      if (moveRight(b)) {
        toggle_game();
        print_string("\n> ");
      }
    } else if (scancode == 0x4B || scancode == 0x1E) {
      if (moveLeft(b)) {
        toggle_game();
        print_string("\n> ");
      }
    } else if (inputKey.UnicodeChar == u'u') {
      toggle_game();
      print_string("\n> ");
    }
    return;
  }
  if (tic_tac_toe) {
    if (scancode >= 0x02 && scancode <= 0x0A) {
      int result = moveTTT(bT, inputKey.UnicodeChar);
      if (result == 1) {
        print_string("Box is alreay occupied!\n");
      } else if (result == 'X') {
        print_string("You have won!\n");
        toggle_tic_tac_toe();
      } else if (result == 'O') {
        print_string("The computer has won!\n");
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
  } else if (scancode == 0x2386) {
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

EFI_STATUS ScrollLn(UINTN offset) {
  EFI_STATUS Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID GraphicsOutputProt;
  Status = uefi_call_wrapper(g_SystemTable->HandleProtocol, 3, g_SystemTable->ConsoleOutHandle, &GraphicsOutputProt, (VOID **)&GraphicsOutput);
  EFI_GRAPHICS_OUTPUT_PROTOCOL = g_SystemTable->
                                 // Calculate number of pixels to scroll
                                 UINTN PixelOffset = offset * GraphicsOutput->Mode->Info->PixelsPerScanLine;

  // Scroll the screen
  Status = GraphicsOutput->Blt(GraphicsOutput, NULL, EfiBltBufferToVideo, 0, PixelOffset, 0, 0, 0, 0, 0);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Clear the last row
  UINT32 White = 0xFFFFFFFF; // Assuming white color
  for (UINTN col = 0; col < GraphicsOutput->Mode->Info->HorizontalResolution; col++) {
    GraphicsOutput->Blt(GraphicsOutput, &White, EfiBltVideoFill, 0, 0, col, GraphicsOutput->Mode->Info->VerticalResolution - 1, 1, 1, 0);
  }

  return EFI_SUCCESS;
}

// EFI Image Entry Point
EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
  (void)ImageHandle; // Prevent compiler warning
  g_SystemTable = SystemTable;
  // Set text to yellow fg/ green bg
  SystemTable->ConOut->SetAttribute(SystemTable->ConOut, EFI_TEXT_ATTR(EFI_BLACK, EFI_WHITE));

  // Clear screen to bg color
  SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
  init_memory();
  // SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Hello, World!\r\n\r\n");

  // Set text to red fg/ black bg
  // SystemTable->ConOut->SetAttribute(SystemTable->ConOut, EFI_TEXT_ATTR(EFI_RED, EFI_BLACK));

  // SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Press any key to shutdown...");
  print_string("test");
  scroll_screen(1);
  // Wait until keypress, then return
  EFI_INPUT_KEY key;
  //  // Loop to continuously read pressed keys
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
  SystemTable->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);

  // Should never get here
  return EFI_SUCCESS;
}
