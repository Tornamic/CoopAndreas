#include "stdafx.h"

ID3DXFont* CDXFont::m_pD3DXFont;

void CDXFont::InitFont() 
{
    m_pD3DXFont = NULL;
    Events::drawingEvent.Add(Draw);
    Events::initRwEvent.Add(InitFont);
    Events::shutdownRwEvent.Add(DestroyFont);
    Events::d3dLostEvent.Add(DestroyFont);
    Events::d3dResetEvent.Add(InitFont);

    IDirect3DDevice9* device = reinterpret_cast<IDirect3DDevice9*>(RwD3D9GetCurrentD3DDevice());
    D3DXCreateFontA(device, 48, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "arial", &m_pD3DXFont);
}

void CDXFont::DestroyFont() 
{
    if (m_pD3DXFont) 
    {
        m_pD3DXFont->Release();
        m_pD3DXFont = NULL;
    }
}

void CDXFont::Draw() 
{
    if (m_pD3DXFont) 
    {
        // screen rectangle
        RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.bottom = RsGlobal.maximumHeight;
        rect.right = RsGlobal.maximumWidth;


        // draw here
        m_pD3DXFont->DrawTextA(NULL, "дрочи мне €йца пЄс", -1, &rect, DT_CENTER | DT_VCENTER, D3DCOLOR_RGBA(255, 255, 0, 255));
    }
}
