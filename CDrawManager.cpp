#include "CDrawManager.h"

//===================================================================================
CDrawManager gDrawManager;

constexpr int ESP_HEIGHT = 14; // Use constexpr for constant values

//===================================================================================
void CDrawManager::Initialize() {
    if (gInts.Surface == nullptr) {
        return;
    }

    // Wait until screen size is retrieved
    while (gScreenSize.iScreenWidth == 0) {
        gInts.Engine->GetScreenSize(gScreenSize.iScreenWidth, gScreenSize.iScreenHeight);
    }

    m_Font = gInts.Surface->CreateFont();
    gInts.Surface->SetFontGlyphSet(m_Font, "Tahoma", ESP_HEIGHT, 500, 0, 0, 0x200);
}

//===================================================================================
void CDrawManager::DrawString(int x, int y, DWORD dwColor, const wchar_t* pszText) {
    if (pszText == nullptr) {
        return;
    }

    gInts.Surface->DrawSetTextPos(x, y);
    gInts.Surface->DrawSetTextFont(m_Font);
    gInts.Surface->DrawSetTextColor(RED(dwColor), GREEN(dwColor), BLUE(dwColor), ALPHA(dwColor));
    gInts.Surface->DrawPrintText(pszText, wcslen(pszText));
}

//===================================================================================
void CDrawManager::DrawString(int x, int y, DWORD dwColor, const char* pszText, ...) {
    if (pszText == nullptr) {
        return;
    }

    va_list va_alist;
    char szBuffer[1024] = { '\0' };
    wchar_t szString[1024] = { '\0' };

    va_start(va_alist, pszText);
    vsnprintf(szBuffer, sizeof(szBuffer), pszText, va_alist); // Use vsnprintf for safety
    va_end(va_alist);

    swprintf(szString, sizeof(szString) / sizeof(wchar_t), L"%S", szBuffer); // Use swprintf for safety

    gInts.Surface->DrawSetTextPos(x, y);
    gInts.Surface->DrawSetTextFont(m_Font);
    gInts.Surface->DrawSetTextColor(RED(dwColor), GREEN(dwColor), BLUE(dwColor), ALPHA(dwColor));
    gInts.Surface->DrawPrintText(szString, wcslen(szString));
}

//===================================================================================
byte CDrawManager::GetESPHeight() const {
    return ESP_HEIGHT;
}

//===================================================================================
void CDrawManager::DrawRect(int x, int y, int w, int h, DWORD dwColor) {
    gInts.Surface->DrawSetColor(RED(dwColor), GREEN(dwColor), BLUE(dwColor), ALPHA(dwColor));
    gInts.Surface->DrawFilledRect(x, y, x + w, y + h);
}

//===================================================================================
void CDrawManager::OutlineRect(int x, int y, int w, int h, DWORD dwColor) {
    gInts.Surface->DrawSetColor(RED(dwColor), GREEN(dwColor), BLUE(dwColor), ALPHA(dwColor));
    gInts.Surface->DrawOutlinedRect(x, y, x + w, y + h);
}

//===================================================================================
void CDrawManager::DrawBox(Vector vOrigin, int r, int g, int b, int alpha, int box_width, int radius) {
    Vector vScreen;

    if (!WorldToScreen(vOrigin, vScreen)) {
        return;
    }

    const int radius2 = radius << 1;

    OutlineRect(vScreen.x - radius + box_width,
        vScreen.y - radius + box_width,
        radius2 - box_width,
        radius2 - box_width,
        0x000000FF);

    OutlineRect(vScreen.x - radius - 1,
        vScreen.y - radius - 1,
        radius2 + (box_width + 2),
        radius2 + (box_width + 2),
        0x000000FF);

    DrawRect(vScreen.x - radius + box_width,
        vScreen.y - radius,
        radius2 - box_width,
        box_width,
        COLORCODE(r, g, b, alpha));

    DrawRect(vScreen.x - radius,
        vScreen.y + radius,
        radius2,
        box_width,
        COLORCODE(r, g, b, alpha));

    DrawRect(vScreen.x - radius,
        vScreen.y - radius,
        box_width,
        radius2,
        COLORCODE(r, g, b, alpha));

    DrawRect(vScreen.x + radius,
        vScreen.y - radius,
        box_width,
        radius2 + box_width,
        COLORCODE(r, g, b, alpha));
}

//===================================================================================
bool CDrawManager::WorldToScreen(Vector& vOrigin, Vector& vScreen) {
    const auto& worldToScreen = gInts.Engine->WorldToScreenMatrix(); // Grab the world to screen matrix

    float w = worldToScreen[3][0] * vOrigin[0] +
        worldToScreen[3][1] * vOrigin[1] +
        worldToScreen[3][2] * vOrigin[2] +
        worldToScreen[3][3]; // Calculate the angle in comparison to the player's camera

    vScreen.z = 0; // Screen doesn't have a 3rd dimension

    if (w > 0.001f) { // If the object is within view
        float fl1DBw = 1.0f / w; // Divide 1 by the angle
        vScreen.x = (gScreenSize.iScreenWidth / 2) +
            (0.5f * ((worldToScreen[0][0] * vOrigin[0] +
                worldToScreen[0][1] * vOrigin[1] +
                worldToScreen[0][2] * vOrigin[2] +
                worldToScreen[0][3]) * fl1DBw) *
                gScreenSize.iScreenWidth + 0.5f)); // Get the X dimension

                vScreen.y = (gScreenSize.iScreenHeight / 2) -
                    (0.5f * ((worldToScreen[1][0] * vOrigin[0] +
                        worldToScreen[1][1] * vOrigin[1] +
                        worldToScreen[1][2] * vOrigin[2] +
                        worldToScreen[1][3]) *
                        fl1DBw) *
                        gScreenSize.iScreenHeight + 0.5f)); // Get the Y dimension
                        return true;
    }

    return false;
}