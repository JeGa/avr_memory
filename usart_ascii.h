#pragma once

#define LF 0x0A
#define CR 0x0D
#define DEL 0x7F
#define ESC 0x1B // '\e'

#define CHAR_SMALL_START 0x61
#define CHAR_SMALL_END 0x7A
#define CHAR_BIG_START 0x41
#define CHAR_BIG_END 0x5A

// Escape sequences
#define QUERY_CURSOR_POS "\e[6n"