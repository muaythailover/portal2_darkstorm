#include "SDK.h"
#include "Client.h"
#include "Panels.h"

// Global Variables
COffsets gOffsets;
CPlayerVariables gPlayerVars;
CInterfaces gInts;

CreateInterface_t EngineFactory = nullptr;
CreateInterface_t ClientFactory = nullptr;
CreateInterface_t VGUIFactory = nullptr;
CreateInterface_t VGUI2Factory = nullptr;

// Main thread function for initializing the client
DWORD WINAPI dwMainThread(LPVOID lpArguments)
{
    if (gInts.Client == nullptr) // Prevent repeat calls
    {
        // Grab the factories from their respective module's EAT
        ClientFactory = reinterpret_cast<CreateInterface_t>(GetProcAddress(gBaseAPI.GetModuleHandleSafe("client.dll"), "CreateInterface"));
        gInts.Client = reinterpret_cast<CHLClient*>(ClientFactory("VClient016", nullptr));
        XASSERT(gInts.Client);

        gInts.EntList = reinterpret_cast<CEntList*>(ClientFactory("VClientEntityList003", nullptr));

        EngineFactory = reinterpret_cast<CreateInterface_t>(GetProcAddress(gBaseAPI.GetModuleHandleSafe("engine.dll"), "CreateInterface"));
        gInts.Engine = reinterpret_cast<EngineClient*>(EngineFactory("VEngineClient015", nullptr));
        XASSERT(gInts.Engine);

        gInts.ModelInfo = reinterpret_cast<CModelInfo*>(EngineFactory("VModelInfoClient004", nullptr));
        XASSERT(gInts.ModelInfo);

        VGUIFactory = reinterpret_cast<CreateInterface_t>(GetProcAddress(gBaseAPI.GetModuleHandleSafe("vguimatsurface.dll"), "CreateInterface"));
        gInts.Surface = reinterpret_cast<ISurface*>(VGUIFactory("VGUI_Surface031", nullptr));
        XASSERT(gInts.Surface);

        VGUI2Factory = reinterpret_cast<CreateInterface_t>(GetProcAddress(gBaseAPI.GetModuleHandleSafe("vgui2.dll"), "CreateInterface"));

        // Setup the ClientMode hooks
        gInts.ClientMode = GetClientMode();

        auto clientMode = std::make_unique<VMTBaseManager>(); // Use smart pointers for automatic memory management
        clientMode->Init(gInts.ClientMode);
        clientMode->HookMethod(&Hooked_CreateMove, gOffsets.iCreateMoveOffset);
        clientMode->Rehook();

        // Setup the Panel hook so we can draw
        if (!gInts.Panels)
        {
            gInts.Panels = reinterpret_cast<IPanel*>(VGUI2Factory("VGUI_Panel009", nullptr));
            XASSERT(gInts.Panels);

            if (gInts.Panels)
            {
                auto panelHook = std::make_unique<VMTBaseManager>(); // Use smart pointers for automatic memory management
                panelHook->Init(gInts.Panels);
                panelHook->HookMethod(&Hooked_PaintTraverse, gOffsets.iPaintTraverseOffset);
                panelHook->Rehook();
            }
        }
    }

    return 0; // The thread has been completed, and we do not need to call anything once we're done.
}

// DLL entry point function
int WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    // If you manually map, make sure you setup this function properly.
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        CreateThread(nullptr, 0, dwMainThread, nullptr, 0, nullptr); // CreateThread > _BeginThread.
    }

    return TRUE; // Indicate successful initialization.
}