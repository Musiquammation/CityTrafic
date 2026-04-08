#pragma once

#include <stdio.h>
extern FILE* debugFile;


#define debugLog(fmt, ...) fprintf(debugFile, fmt, ##__VA_ARGS__)