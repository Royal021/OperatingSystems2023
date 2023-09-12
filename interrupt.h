#pragma once

#include "utils.h"

extern void* interrupt_table_begin;
extern void* interrupt_table_end;
void interrupt_init();