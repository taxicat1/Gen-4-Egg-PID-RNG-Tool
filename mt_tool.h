#ifndef MT_TOOL_H
#define MT_TOOL_H

#include <stdint.h>

void mtSrand(uint32_t s);
uint32_t mtTemper(uint32_t y);
uint32_t mtUntemper(uint32_t v);
uint32_t* mtGetBuf();
extern const unsigned int mtLen;

#endif
