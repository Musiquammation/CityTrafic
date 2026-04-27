#pragma once

#include <stdio.h>
extern FILE* debugFile;

#if defined(MAIN_TEST_ID) &&  MAIN_TEST_ID == 0

#define debugLog(fmt, ...) fprintf(debugFile, fmt, ##__VA_ARGS__)

#else

#define debugLog(fmt, ...) 

#endif
// #define debugLog(fmt, ...) printf(fmt, ##__VA_ARGS__)
