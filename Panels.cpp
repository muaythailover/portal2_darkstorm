#include "SDK.h"
#include "Panels.h"
#include "Client.h"
#include "CDrawManager.h"

CScreenSize gScreenSize;

//===================================================================================
void __fastcall Hooked_PaintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
    try
    {
        // Get a pointer to the instance of VMTManager
        VMTManager& hook = VMTManager::GetHook(pPanels);
        hook.GetMethod<void(__thiscall*)(PVOID, unsigned int, bool, bool)>(gOffsets.iPaintTraverseOffset)(pPanels, vguiPanel, forceRepaint, allowForce); // Call the original.

        static unsigned int vguiMatSystemTopPanel = 0;

        // Check if we are on the MatSystemTopPanel
        if (vguiMatSystemTopPanel == 0)
        {
            const char* szName = gInts.Panels->GetName(vguiPanel);
            if (szName[0] == 'M' && szName[3] == 'S') // Look for MatSystemTopPanel
            {
                vguiMatSystemTopPanel = vguiPanel;
                Intro();
            }
        }

        // If we're on MatSystemTopPanel, call our drawing code
        if (vguiMatSystemTopPanel == vguiPanel)
        {
            if (gInts.Engine->IsDrawingLoadingImage() ||
                !gInts.Engine->IsInGame() ||
                !gInts.Engine->IsConnected() ||
                gInts.Engine->Con_IsVisible() ||
                (GetAsyncKeyState(VK_F12) || gInts.Engine->IsTakingScreenshot()))
            {
                return; // Don't draw at the menu.
            }

            // Uncomment this line if you want to display a welcome message
            // gDrawManager.DrawString((gScreenSize.iScreenWidth / 2) - 55, 200, gDrawManager.dwGetTeamColor(3), "Welcome to Darkstorm");

            if (GetAsyncKeyState(VK_F10) & 0x8000)
            {
                DrawEntities(true); // Draw entities with detailed information
            }
            else
            {
                DrawEntities(false); // Draw entities with basic information
            }
        }
    }
    catch (...)
    {
        gBaseAPI.LogToFile("Failed PaintTraverse");
        gBaseAPI.ErrorBox("Failed PaintTraverse");
    }
}

// Function to draw entities based on the detailed flag
void DrawEntities(bool detailed)
{
    Vector vScreen, vWorldPos;

    for (int iIndex = 0; iIndex <= gInts.EntList->GetHighestEntityIndex(); iIndex++)
    {
        CBaseEntity* pBaseEntity = GetBaseEntity(iIndex);
        if (!pBaseEntity)
            continue;

        pBaseEntity->GetWorldSpaceCenter(vWorldPos);
        if (!gDrawManager.WorldToScreen(vWorldPos, vScreen))
            continue;

        int iID = pBaseEntity->GetClientClass()->iClassID;

        if (detailed)
        {
            // Detailed entity logging
            gDrawManager.DrawString(vScreen.x, vScreen.y, 0xFFFFFFFF, "%s", gInts.ModelInfo->GetModelName(pBaseEntity->GetModel()));

            static bool bLogEnts = false;
            if (!bLogEnts)
            {
                LogEntityClasses();
                bLogEnts = true;
            }
        }
        else
        {
            // Basic entity drawing based on ID
            switch (iID)
            {
            case 2: // Button or Switch
                if (strstr(gInts.ModelInfo->GetModelName(pBaseEntity->GetModel()), "button") ||
                    strstr(gInts.ModelInfo->GetModelName(pBaseEntity->GetModel()), "switch"))
                {
                    gDrawManager.DrawString(vScreen.x - 15, vScreen.y - 6, 0xFF0000FF, "Button");
                }
                break;

            case 108: // Player
                gDrawManager.DrawString(vScreen.x - 15, vScreen.y - 6, 0x00FFFFFF, pBaseEntity->GetPlayerName());
                break;

            case 88: // Personality Core
                DrawPersonalityCore(pBaseEntity, vScreen);
                break;

            case 136: // Box or Reflector Box
                DrawBoxEntities(pBaseEntity, vScreen);
                break;

            case 99: // Turret or Box
                DrawTurretEntities(pBaseEntity, vScreen);
                break;

            case 87: // Defective Turret
            case 90: // Turret
            case 89: // Turret
                DrawDefectiveTurret(pBaseEntity, vScreen);
                break;

            default:
                break;
            }
        }
    }
}

// Function to log entity classes to file
void LogEntityClasses()
{
    gBaseAPI.LogToFile("Begin logging Classes");
    for (ClientClass* pCC = gInts.Client->GetAllClasses(); pCC; pCC = pCC->pNextClass)
    {
        gBaseAPI.LogToFile("Class: %s Recv: %s ID: %i", pCC->chName, pCC->Table->GetName(), pCC->iClassID);
    }
    gBaseAPI.LogToFile("End of logging Classes");
}

// Function to draw personality core names based on skin type
void DrawPersonalityCore(CBaseEntity* pBaseEntity, const Vector& vScreen)
{
    switch (pBaseEntity->GetSkin())
    {
    case 0:
        gDrawManager.DrawString(vScreen.x - 30, vScreen.y - 6, 0x0080FFFF, "Wheatly");
        break;
    case 1:
        gDrawManager.DrawString(vScreen.x - 15, vScreen.y - 6, 0x00FF00FF, "Rick");
        break;
    case 2:
        gDrawManager.DrawString(vScreen.x - 35, vScreen.y - 6, 0xFFED24FF, "Space Core");
        break;
    case 3:
        gDrawManager.DrawString(vScreen.x - 30, vScreen.y - 6, 0xFF7FEDFF, "Fact Core");
        break;
    }
}

// Function to handle drawing box entities based on model name and skin type
void DrawBoxEntities(CBaseEntity* pBaseEntity, const Vector& vScreen)
{
    const char* modelName = gInts.ModelInfo->GetModelName(pBaseEntity->GetModel());

    if (strstr(modelName, "metal_box.mdl"))
    {
        switch (pBaseEntity->GetSkin())
        {
        case 0:
        case 2:
        case 5:
            gDrawManager.DrawString(vScreen.x - 15, vScreen.y - 6, 0x0000FFFF, "Box");
            return;

        case 1:
            gDrawManager.DrawString(vScreen.x - 15, vScreen.y - 6, 0xFF7FEDFF, "Companion Cube");
            return;

        case 3:
            gDrawManager.DrawString(vScreen.x - 15, vScreen.y - 6, 0x0000FFFF, "Reflector Box");
            return;

        case 4:
            gDrawManager.DrawString(vScreen.x - 15, vScreen.y - 6, 0x0000FFFF, "Sphere");
            return;
        }

        // Default box label if no specific skin matched
        gDrawManager.DrawString(vScreen.x - 15, vScreen.y - 6, 0x0000FFFF, "Box");
    }
    else if (strstr(modelName, "reflection_cube.mdl"))
    {
        gDrawManager.DrawString(vScreen.x - 15, vScreen.y - 6, 0x0000FFFF, "Reflector Box");
    }
    else if (strstr(modelName, "underground_weighted_cube.mdl"))
    {
        gDrawManager.DrawString(vScreen.x - 15, vScreen.y - 6, 0x0000FFFF, "Box");
    }
}

// Function to handle drawing turret entities based on model name
void DrawTurretEntities(CBaseEntity* pBaseEntity, const Vector& vScreen)
{
    const char* modelName = gInts.ModelInfo->GetModelName(pBaseEntity->GetModel());

    if (strstr(modelName, "monster_a.mdl"))
    {
        gDrawManager.DrawString(vScreen.x - 15, vScreen.y - 6, 0x0000FFFF, "Frankenturret");
    }
    else if (strstr(modelName, "monster_A_box"))
    {
        gDrawManager.DrawString(vScreen.x - 15, vScreen.y - 6, 0x0000FFFF, "Box");
    }
}

// Function to handle drawing defective turret entities based on model name and skin type
void DrawDefectiveTurret(CBaseEntity* pBaseEntity, const Vector& vScreen)
{
    const char* modelName = gInts.ModelInfo->GetModelName(pBaseEntity->GetModel());

    if (strstr(modelName, "skeleton") || strstr(modelName, "backwards") || pBaseEntity->GetSkin() == 1)
    {
        gDrawManager.DrawString(vScreen.x - 15, vScreen.y - 6, 0x808080FF, "Defective Turret");
    }
    else
    {
        gDrawManager.DrawString(vScreen.x - 15, vScreen.y - 6, 0xFF8000FF, "Turret");
    }
}

//===================================================================================
void Intro()
{
    try
    {
        gDrawManager.Initialize(); // Initialize the drawing class.

        // Uncomment this line to log successful injection message.
        // gBaseAPI.LogToFile("Injection Successful"); 
    }
    catch (...)
    {
        gBaseAPI.LogToFile("Failed Intro");
        gBaseAPI.ErrorBox("Failed Intro");
    }
}