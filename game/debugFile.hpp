#pragma once

#include <stdio.h>
extern FILE* debugFile;


#define debugLog(fmt, ...) fprintf(debugFile, fmt, ##__VA_ARGS__)
// #define debugLog(fmt, ...) printf(fmt, ##__VA_ARGS__)
// #define debugLog(fmt, ...) {};
