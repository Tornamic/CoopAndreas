#include "stdafx.h"

ID3DXFont* CDXFont::m_pD3DXFont;

void CDXFont::InitFont() 
{
    IDirect3DDevice9* device = reinterpret_cast<IDirect3DDevice9*>(RwD3D9GetCurrentD3DDevice());
    D3DXCreateFontA(device, RENDER_FONT_SIZE, 0, 400, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "consolas", &m_pD3DXFont);
}

void CDXFont::DestroyFont() 
{
    if (m_pD3DXFont) 
    {
        m_pD3DXFont->Release();
        m_pD3DXFont = NULL;
    }
}

void CDXFont::Draw(int x, int y, const char* text, D3DCOLOR color)
{
    if (m_pD3DXFont)
    {
        RECT rect{};
        rect.left = x;
        rect.top = y;
        rect.bottom = RsGlobal.maximumHeight;
        rect.right = RsGlobal.maximumWidth;

        const int shadowSize = 1;
        // right shadow 
        rect.left += shadowSize;
        CDXFont::m_pD3DXFont->DrawTextA(NULL, text, -1, &rect, 0, D3DCOLOR_RGBA(0, 0, 0, 255)); 
        rect.left -= shadowSize;

        // left shadow 
        rect.left -= shadowSize;
        CDXFont::m_pD3DXFont->DrawTextA(NULL, text, -1, &rect, 0, D3DCOLOR_RGBA(0, 0, 0, 255));
        rect.left += shadowSize;

        // bottom shadow
        rect.top += shadowSize;
        CDXFont::m_pD3DXFont->DrawTextA(NULL, text, -1, &rect, 0, D3DCOLOR_RGBA(0, 0, 0, 255));
        rect.top -= shadowSize;
        
        // top shadow
        rect.top -= shadowSize;
        CDXFont::m_pD3DXFont->DrawTextA(NULL, text, -1, &rect, 0, D3DCOLOR_RGBA(0, 0, 0, 255));
        rect.top += shadowSize;

        CDXFont::m_pD3DXFont->DrawTextA(NULL, text, -1, &rect, 0, color);
    }
}

void CDXFont::Init()
{
    m_pD3DXFont = NULL;
    Events::initRwEvent.Add(InitFont);
    Events::shutdownRwEvent.Add(DestroyFont);
    Events::d3dLostEvent.Add(DestroyFont);
    Events::d3dResetEvent.Add(InitFont);
}