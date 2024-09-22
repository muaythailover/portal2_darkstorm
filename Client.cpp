#include "SDK.h"
#include "Client.h"
#include <optional>
#include <stdexcept>

//============================================================================================
void __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand)
{
    try
    {
        // Grab the local player's entity pointer.
        auto pBaseEntity = GetBaseEntity(me);

        // Use std::optional for better null handling
        if (!pBaseEntity) {
            throw std::runtime_error("Failed to retrieve base entity.");
        }

        // Get a pointer to the instance of VMTManager
        auto& hook = VMTManager::GetHook(gInts.ClientMode);
        hook.GetMethod<void(__thiscall*)(PVOID, float, CUserCmd*)>(gOffsets.iCreateMoveOffset)(gInts.ClientMode, input_sample_frametime, pCommand); // Call the original.

        // Do your client hook stuff here. This function is called once per tick.
        int iFlags = *reinterpret_cast<PDWORD>(reinterpret_cast<uintptr_t>(pBaseEntity) + 0xF8);

        // Bunnyhop implementation
        if (pCommand->buttons & IN_JUMP)
        {
            pCommand->buttons &= ~IN_JUMP;
            if (iFlags & FL_ONGROUND)
            {
                pCommand->buttons |= IN_JUMP;
            }
        }
    }
    catch (...)
    {
        gBaseAPI.LogToFile("Failed Hooked_CreateMove: Unknown exception");
    }
}
//============================================================================================
const char* CBaseEntity::GetPlayerName()
{
    using GetGameResources_t = IGameResources * (__stdcall*)(void);

    static const auto GetGameResourcesCall = reinterpret_cast<GetGameResources_t>(
        gBaseAPI.GetClientSignature("A1 ? ? ? ? 85 C0 74 06 05"));

    if (GetGameResourcesCall) {
        return GetGameResourcesCall()->GetPlayerName(this->GetIndex());
    }

    return NULL; // Return an empty optional if the call fails
}
//============================================================================================
PDWORD GetClientMode()
{
    // Because Portal 2 uses split screen, I need to call this wrapper function.
    static const DWORD clientModeAddress = gBaseAPI.dwGetCallFunctionLocation(
        gBaseAPI.GetClientSignature("F3 0F 11 80 ? ? ? ? E8 ? ? ? ? 8B 10 F3 0F 10 45") + 8);

    using ClientModeFn = PDWORD(__stdcall*)(void);

    static const auto ClientMode = reinterpret_cast<ClientModeFn>(clientModeAddress);

    if (ClientMode) {
        return ClientMode();
    }

    return std::nullptr_t; // Return an empty optional if the call fails
}