#pragma once

#include <stdio.h>
#include <windows.h>

#define DEBUG(...) {char cad[512]; sprintf(cad, __VA_ARGS__);  OutputDebugString(cad);}