#ifndef _TLM1638_1_H
#define _TLM1638_1_H
#include <stdint.h>
void TLMsendCommand(uint8_t value);
void TLMreset();
void TLMsetup();
void TLMloop();
void TLMloop1();

#endif
