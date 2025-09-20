#ifndef _MACRO_PRINT_
#define _MACRO_PRINT_

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define INFO(fmt, ...) printf(ANSI_COLOR_GREEN "[INFO] " ANSI_COLOR_RESET fmt "\n" ,##__VA_ARGS__)
#define WARNING(fmt, ...) printf(ANSI_COLOR_RED "[WARNING] " ANSI_COLOR_RESET fmt "\n" ,##__VA_ARGS__)
#define UTEST(fmt, ...) printf(ANSI_COLOR_YELLOW "[TEST] " ANSI_COLOR_RESET fmt "\n",##__VA_ARGS__)
#define SUCCESS(fmt, ...) printf(ANSI_COLOR_GREEN "[SUCCESS] " ANSI_COLOR_RESET fmt "\n" ,##__VA_ARGS__)

#define BYPE 1 // 1 bype

#define EQ_STR(str1,str2) strcmp(str1,str2) == 0 ? 1 : 0
#define NOT_OVERFLOW(i,capacity) (((i) + 1) >= (capacity) ? 0 : 1)

#define INVALID_ -1
#define is_invalid(test) ((test) == INVALID_ ? 1 : 0) 

#endif