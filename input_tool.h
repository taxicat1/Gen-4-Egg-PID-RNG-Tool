#ifndef INPUT_TOOL_H
#define INPUT_TOOL_H

#include <stdint.h>

int inputReadInt(const char* prompt, int min, int max);
uint32_t inputReadU32Hex(const char* prompt);

#endif
