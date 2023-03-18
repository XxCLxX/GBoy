#pragma once
#include <common.h>
// https://gbdev.io/pandocs/OAM_DMA_Transfer.html

void dma_start_address(u8 start); //Register
void dma_tick();
bool dma_transfer();