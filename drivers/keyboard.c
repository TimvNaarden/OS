#include <stdbool.h>
#include "keyboard.h"
#include "ports.h"
#include "../cpu/isr.h"
#include "display.h"
#include "../kernel/util.h"
#include "../kernel/kernel.h"
#include "../games/2048.h"
#include "../games/tic_tac_toe.h"

#define BACKSPACE 0x0E
#define ENTER 0x1C
#define SC_MAX 57

int game = 0;
int tic_tac_toe = 0;

TTT_Board* bT;
Board* b;

static char key_buffer[256];

void toggle_game() {
    if (game) {
        mem_free(b);
        game = 0;
        
    }
    else if (!game) {
        b = (Board*)mem_alloc(sizeof(Board));
        game = 1;
        initBoard(b);
        printBoard(b);
    }
}

void toggle_tic_tac_toe() {
    if (tic_tac_toe) {
        mem_free(bT);
        tic_tac_toe = 0;
        print_string("> ");
    }
    else if (!tic_tac_toe) {
        bT = (TTT_Board*)mem_alloc(sizeof(TTT_Board));
        tic_tac_toe = 1;
        initTTT_Board(bT);
        printTTT_Board(bT);
    }
}


const char *sc_name[] = {"ERROR", "Esc", "1", "2", "3", "4", "5", "6",
                         "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E",
                         "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl",
                         "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`",
                         "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".",
                         "/", "RShift", "Keypad *", "LAlt", "Spacebar"};
const char sc_ascii[] = {'?', '?', '1', '2', '3', '4', '5', '6',
                         '7', '8', '9', '0', '-', '=', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y',
                         'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G',
                         'H', 'J', 'K', 'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V',
                         'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '};

static void keyboard_callback(registers_t *regs) {
    uint8_t scancode = port_byte_in(0x60);
    if (scancode > SC_MAX) return;
    if (game) {
        if (scancode == 0x48 || scancode == 0x11){
            if(moveUp(b)) {
                toggle_game();
                print_string("\n> ");
            }
        } else if (scancode == 0x50|| scancode == 0x1F){
            if(moveDown(b)) {
                toggle_game();
                print_string("\n> ");
            }
        }else if (scancode == 0x4D || scancode == 0x20){
            if(moveRight(b)) {
                toggle_game();
                print_string("\n> ");
            }
        }else if (scancode == 0x4B || scancode == 0x1E){
            if(moveLeft(b)) {
                toggle_game();
                print_string("\n> ");
            }
        }else if(sc_ascii[(int) scancode] == 'Q') {
            toggle_game();
            print_string("\n> ");
        }
        return;
    }
    if(tic_tac_toe) {
        if(scancode >= 0x02 && scancode <= 0x0A) {
            int result = moveTTT(bT, sc_ascii[(int) scancode]);
            if(result == 1){
                print_string("Box is alreay occupied!\n");
            } else if(result == 'X') {
                print_string("You have won!\n");
                toggle_tic_tac_toe();
            } else if(result == 'O') {
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
    
    if (scancode == BACKSPACE) {
        if (backspace(key_buffer)) {
            print_backspace();
        }
    } else if (scancode == ENTER) {
        print_nl();
        execute_command(key_buffer);
        key_buffer[0] = '\0';

    } else {
        char letter = sc_ascii[(int) scancode];
        append(key_buffer, letter);
        char str[2] = {letter, '\0'};
        print_string(str);
        
    }
}

void init_keyboard() {
    register_interrupt_handler(IRQ1, keyboard_callback);
}
