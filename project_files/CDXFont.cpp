#include "stdafx.h"
#include "CDXFont.h"

ID3DXFont* CDXFont::m_pD3DXFont;
uint8_t CDXFont::m_fFontSize;

void CDXFont::InitFont() 
{
    IDirect3DDevice9* device = reinterpret_cast<IDirect3DDevice9*>(RwD3D9GetCurrentD3DDevice());

    const float normalizedWidth = std::clamp(((float)RsGlobal.maximumWidth - MIN_SCREEN_WIDTH) / (MAX_SCREEN_WIDTH - MIN_SCREEN_WIDTH), 0.0f, 1.0f);
    const float normalizedHeight = std::clamp(((float)RsGlobal.maximumHeight - MIN_SCREEN_HEIGHT) / (MAX_SCREEN_HEIGHT - MIN_SCREEN_HEIGHT), 0.0f, 1.0f);
    const float normalizedScale = min(normalizedWidth, normalizedHeight);

    const float fontSize = MIN_RENDER_FONT_SIZE + normalizedScale * (MAX_RENDER_FONT_SIZE - MIN_RENDER_FONT_SIZE);
    CDXFont::m_fFontSize = (uint8_t)fontSize;

    D3DXCreateFontA(device, (INT)fontSize, 0, 400, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "consolas", &m_pD3DXFont);
}

void CDXFont::DestroyFont() 
{
    if (!m_pD3DXFont) 
        return;

    m_pD3DXFont->Release();
    m_pD3DXFont = NULL;
}

void CDXFont::Draw(int x, int y, const std::vector<CTextSegment>& segments)
{
    if (!m_pD3DXFont)
        return;

    int currentX = x;
    for (auto& seg : segments)
    {
        DrawSegmentWithShadow(currentX, y, seg.text.c_str(), seg.color);
        currentX += GetTextWidth(seg.text);
    }
}

void CDXFont::Draw(int x, int y, const char* rawText, D3DCOLOR defaultColor)
{
    if (!m_pD3DXFont)
        return;

    std::vector<CTextSegment> segments = ParseColorSegments(rawText, defaultColor);
    Draw(x, y, segments);
}

void CDXFont::DrawSegmentWithShadow(int x, int y, const char* text, D3DCOLOR color)
{
    if (!m_pD3DXFont)
        return;

    uint8_t shadowThickness = m_iShadowSize;

    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    uint8_t a = (color >> 24) & 0xFF;
    
    D3DCOLOR shadowColor = D3DCOLOR_RGBA(0, 0, 0, a);

    if (r + g + b < 204)
    {
        shadowThickness = 0;
    }

    RECT rect{};
    rect.left = x;
    rect.top = y;
    rect.right = RsGlobal.maximumWidth;
    rect.bottom = RsGlobal.maximumHeight;

    if (shadowThickness > 0)
    {
        RECT rightRect = rect;
        rightRect.left += shadowThickness;
        m_pD3DXFont->DrawTextA(nullptr, text, -1, &rightRect, 0, shadowColor);

        RECT leftRect = rect;
        leftRect.left -= shadowThickness;
        m_pD3DXFont->DrawTextA(nullptr, text, -1, &leftRect, 0, shadowColor);

        RECT bottomRect = rect;
        bottomRect.top += shadowThickness;
        m_pD3DXFont->DrawTextA(nullptr, text, -1, &bottomRect, 0, shadowColor);

        RECT topRect = rect;
        topRect.top -= shadowThickness;
        m_pD3DXFont->DrawTextA(nullptr, text, -1, &topRect, 0, shadowColor);
    }

    m_pD3DXFont->DrawTextA(nullptr, text, -1, &rect, 0, color);
}

int CDXFont::GetTextWidth(const std::string& text)
{
    if (!m_pD3DXFont)
        return 0;

    RECT rc = {0, 0, 0, 0};
    m_pD3DXFont->DrawTextA(nullptr, text.c_str(), -1, &rc, DT_CALCRECT, 0xFFFFFFFF);
    return rc.right - rc.left;
}

std::vector<CTextSegment> CDXFont::ParseColorSegments(const std::string& input, D3DCOLOR defaultColor)
{
    std::vector<CTextSegment> segments;
    D3DCOLOR currentColor = defaultColor;
    std::string buffer;

    size_t i = 0;
    while (i < input.size())
    {
        if (input[i] == '{')
        {
            size_t remaining = input.size() - i - 1;
            bool isSix = (remaining >= 6 && (i + 7) < input.size() && input[i + 7] == '}');
            bool isEight = (remaining >= 8 && (i + 9) < input.size() && input[i + 9] == '}');

            size_t hexLen = 0;
            if (isSix) hexLen = 6;
            if (isEight) hexLen = 8;

            if (hexLen > 0)
            {
                bool allHex = true;
                for (size_t k = i + 1; k < i + 1 + hexLen; k++)
                {
                    if (!std::isxdigit((uint8_t)(input[k])))
                    {
                        allHex = false;
                        break;
                    }
                }

                if (allHex)
                {
                    size_t spaceStart = buffer.size();
                    while (spaceStart > 0 && buffer[spaceStart - 1] == ' ')
                    {
                        spaceStart--;
                    }

                    if (spaceStart < buffer.size())
                    {
                        std::string trailingSpaces = buffer.substr(spaceStart, buffer.size() - spaceStart);
                        buffer.erase(spaceStart, buffer.size() - spaceStart);

                        if (!buffer.empty())
                        {
                            segments.push_back({ buffer, currentColor });
                            buffer.clear();
                        }

                        buffer = trailingSpaces;
                    }
                    else
                    {
                        if (!buffer.empty())
                        {
                            segments.push_back({ buffer, currentColor });
                            buffer.clear();
                        }
                    }

                    std::string hexColor = input.substr(i + 1, hexLen);

                    int colorValue = 0;
                    std::stringstream ss;
                    ss << std::hex << hexColor;
                    ss >> colorValue;

                    uint8_t r = (colorValue >> 16) & 0xFF;
                    uint8_t g = (colorValue >> 8) & 0xFF;
                    uint8_t b = colorValue & 0xFF;
                    uint8_t a = (defaultColor >> 24) & 0xFF;

                    if (hexLen == 8)
                    {
                        r = (colorValue >> 24) & 0xFF;
                        g = (colorValue >> 16) & 0xFF;
                        b = (colorValue >> 8) & 0xFF;
                        a = colorValue & 0xFF;
                    }

                    currentColor = D3DCOLOR_RGBA(r, g, b, a);

                    i += (1 + hexLen + 1);
                    continue;
                }
            }
        }

        buffer.push_back(input[i]);
        i++;
    }

    if (!buffer.empty())
    {
        segments.push_back({ buffer, currentColor });
        buffer.clear();
    }

    return segments;
}

void CDXFont::Init()
{
    m_pD3DXFont = NULL;
    Events::initRwEvent.Add(InitFont);
    Events::shutdownRwEvent.Add(DestroyFont);
    Events::d3dLostEvent.Add(DestroyFont);
    Events::d3dResetEvent.Add(InitFont);
}