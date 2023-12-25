#include "from_morse_encoder.h"
#include <stdio.h>

int collected_symbols = 0;
char prev_symbols[5];

const char MORSE_LETTERS[26][5] = {
    {'.', '-', '#', '#', '#'},
    {'-', '.', '.', '.', '#'},
    {'-', '.', '-', '.', '#'},
    {'-', '.', '.', '#', '#'},
    {'.', '#', '#', '#', '#'},
    {'.', '.', '-', '.', '#'},
    {'-', '-', '.', '#', '#'},
    {'.', '.', '.', '.', '#'},
    {'.', '.', '#', '#', '#'},
    {'.', '-', '-', '-', '#'},
    {'-', '.', '-', '#', '#'},
    {'.', '-', '.', '.', '#'},
    {'-', '-', '#', '#', '#'},
    {'-', '.', '#', '#', '#'},
    {'-', '-', '-', '#', '#'},
    {'.', '-', '-', '.', '#'},
    {'-', '-', '.', '-', '#'},
    {'.', '-', '.', '#', '#'},
    {'.', '.', '.', '#', '#'},
    {'-', '#', '#', '#', '#'},
    {'.', '.', '-', '#', '#'},
    {'.', '.', '.', '-', '#'},
    {'.', '-', '-', '#', '#'},
    {'-', '.', '.', '-', '#'},
    {'-', '.', '-', '-', '#'},
    {'-', '-', '.', '.', '#'}
};

void recognise_letter() {
    for (int i = 0; i < 26; i++) {
        char equal = 1;
        for (int j = 0; j < 5; j++) {
            if (j < collected_symbols && MORSE_LETTERS[i][j] != prev_symbols[j]) {
                equal = 0;
            }
            else if (j >= collected_symbols && MORSE_LETTERS[i][j] != '#') {
                equal = 0;
            }
        }

        if (equal) {
            printf("%c", 'A' + i);
            break;
        }
    }
}

void feed_symbol(char symbol) {
    if (symbol == ' ') {
        recognise_letter();
        collected_symbols = 0;
    }
    else {
        prev_symbols[collected_symbols++] = symbol;
    }
}