#pragma once

#include "SDK.h"

void __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand);
void GetClassIDs ( );
PDWORD GetClientMode();