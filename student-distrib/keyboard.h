#ifndef _KEYBOARD_H
#define _KEYBOARD_H

extern void keyboard_init();
extern void keyboard_handler();

const unsigned char key_map[] = {
    0,  // Not a valid character for index 0
    27, // Escape
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    '-',
    '=',
    8,  // Backspace
    9,  // Tab
    'q',
    'w',
    'e',
    'r',
    't',
    'y',
    'u',
    'i',
    'o',
    'p',
    '[',
    ']',
    13,  // Enter
    17,  // Left Control
    'a',
    's',
    'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',
    ';',
    '\'',
    '`',
    16,  // Left Shift
    '\\',
    'z',
    'x',
    'c',
    'v',
    'b',
    'n',
    'm',
    ',',
    '.',
    '/',
    16,  // Right Shift
    '*',
    18,  // Left Alt
    ' ',
    20,  // Caps Lock
    0,  // F1 (Not a printable character)
    0,  // F2 (Not a printable character)
    0,  // F3 (Not a printable character)
    0,  // F4 (Not a printable character)
    0,  // F5 (Not a printable character)
    0,  // F6 (Not a printable character)
    0,  // F7 (Not a printable character)
    0,  // F8 (Not a printable character)
    0,  // F9 (Not a printable character)
    0,  // F10 (Not a printable character)
    25,  // Num Lock
    0,  // Scroll Lock
    16,  // Home (7 on the numpad)
    24,  // Up Arrow (8 on the numpad)
    25,  // Page Up (9 on the numpad)
    '-', // Keypad -
    16,  // Left Arrow (4 on the numpad)
    '5',
    29,  // Right Arrow (6 on the numpad)
    '+', // Keypad +
    16,  // End (1 on the numpad)
    18,  // Down Arrow (2 on the numpad)
    25,  // Page Down (3 on the numpad)
    18,  // Insert (0 on the numpad)
    18,  // Delete (Del on the numpad)
    0,  // F11 (Not a printable character)
    0,  // F12 (Not a printable character)
    // Fill the rest of the array with zeros for unassigned indexes
};

const unsigned char key_map_shifted[] = {
    0,  // Not a valid character for index 0
    27, // Escape
    '!', // Shifted '1'
    '@', // Shifted '2'
    '#', // Shifted '3'
    '$', // Shifted '4'
    '%', // Shifted '5'
    '^', // Shifted '6'
    '&', // Shifted '7'
    '*', // Shifted '8'
    '(', // Shifted '9'
    ')', // Shifted '0'
    '_', // Shifted '-'
    '+', // Shifted '='
    8,  // Backspace
    9,  // Tab
    'Q',
    'W',
    'E',
    'R',
    'T',
    'Y',
    'U',
    'I',
    'O',
    'P',
    '{', // Shifted '['
    '}', // Shifted ']'
    13,  // Enter
    17,  // Left Control
    'A',
    'S',
    'D',
    'F',
    'G',
    'H',
    'J',
    'K',
    'L',
    ':', // Shifted ';'
    '"', // Shifted '\''
    '~', // Shifted '`'
    16,  // Left Shift
    '|', // Shifted '\'
    'Z',
    'X',
    'C',
    'V',
    'B',
    'N',
    'M',
    '<', // Shifted ','
    '>', // Shifted '.'
    '?', // Shifted '/'
    16,  // Right Shift
    '*', // Keypad *
    18,  // Left Alt
    ' ',
    20,  // Caps Lock
    0,  // F1 (Not a printable character)
    0,  // F2 (Not a printable character)
    0,  // F3 (Not a printable character)
    0,  // F4 (Not a printable character)
    0,  // F5 (Not a printable character)
    0,  // F6 (Not a printable character)
    0,  // F7 (Not a printable character)
    0,  // F8 (Not a printable character)
    0,  // F9 (Not a printable character)
    0,  // F10 (Not a printable character)
    25,  // Num Lock
    0,  // Scroll Lock
    16,  // Home (7 on the numpad)
    24,  // Up Arrow (8 on the numpad)
    25,  // Page Up (9 on the numpad)
    '-', // Keypad -
    16,  // Left Arrow (4 on the numpad)
    '5',
    29,  // Right Arrow (6 on the numpad)
    '+', // Keypad +
    16,  // End (1 on the numpad)
    18,  // Down Arrow (2 on the numpad)
    25,  // Page Down (3 on the numpad)
    18,  // Insert (0 on the numpad)
    18,  // Delete (Del on the numpad)
    0,  // F11 (Not a printable character)
    0,  // F12 (Not a printable character)
    // Fill the rest of the array with zeros for unassigned indexes
};
