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

void CDXFont::Draw(int x, int y, const char* text, D3DCOLOR defaultColor)
{
    if (!m_pD3DXFont)
        return;

    std::vector<CTextSegment> segments = GetSegments(text, defaultColor);
    int currentX = x;

    for (auto& seg : segments)
    {
        DrawSegmentWithShadow(currentX, y, seg.text.c_str(), seg.color);

        int width = CDXFont::GetSegmentWidth(seg.text);
        currentX += width;
    }
}

void CDXFont::DrawSegmentWithShadow(int x, int y, const char* text, D3DCOLOR color)
{
    if (!CDXFont::m_pD3DXFont)
        return;

    uint8_t shadowSize = 1;

    uint8_t red = (color >> 16) & 0xFF;
    uint8_t green = (color >> 8) & 0xFF;
    uint8_t blue = color & 0xFF;
    uint8_t alpha = (color >> 24) & 0xFF;
    D3DCOLOR shadowColor = D3DCOLOR_RGBA(0, 0, 0, alpha);

    if (red + green + blue < 204)
    {
        shadowSize = 0;
    }

    RECT rect{};
    rect.left = x;
    rect.top = y;
    rect.right = RsGlobal.maximumWidth;
    rect.bottom = RsGlobal.maximumHeight;

    RECT rightRect = rect;
    rightRect.left += shadowSize;
    CDXFont::m_pD3DXFont->DrawTextA(nullptr, text, -1, &rightRect, 0, shadowColor);

    RECT leftRect = rect;
    leftRect.left -= shadowSize;
    CDXFont::m_pD3DXFont->DrawTextA(nullptr, text, -1, &leftRect, 0, shadowColor);

    RECT bottomRect = rect;
    bottomRect.top += shadowSize;
    CDXFont::m_pD3DXFont->DrawTextA(nullptr, text, -1, &bottomRect, 0, shadowColor);

    RECT topRect = rect;
    topRect.top -= shadowSize;
    CDXFont::m_pD3DXFont->DrawTextA(nullptr, text, -1, &topRect, 0, shadowColor);

    CDXFont::m_pD3DXFont->DrawTextA(nullptr, text, -1, &rect, 0, color);
}

int CDXFont::GetSegmentWidth(const std::string& seg)
{
    if (!m_pD3DXFont)
        return 0;

    RECT rc = { 0,0,0,0 };
    CDXFont::m_pD3DXFont->DrawTextA(nullptr, seg.c_str(), -1, &rc, DT_CALCRECT, 0xFFFFFFFF);
    return rc.right - rc.left;
}

std::vector<CTextSegment> CDXFont::GetSegments(const std::string& input, D3DCOLOR defaultColor)
{
    if (!m_pD3DXFont)
        return {};

    std::vector<CTextSegment> segments;
    D3DCOLOR currentColor = defaultColor;

    std::string buffer;

    for (size_t i = 0; i < input.size(); )
    {
        if (input[i] == '{')
        {
            size_t remaining = input.size() - i - 1;
            bool isSix = remaining >= 6 && input[i + 7] == '}';
            bool isEight = remaining >= 8 && input[i + 9] == '}';

            size_t hexLength = 0;

            if (isSix)
            {
                hexLength = 6;
            }
            else if (isEight)
            {
                hexLength = 8;
            }

            if (hexLength > 0)
            {
                bool allHex = true;
                for (size_t k = i + 1; k < i + 1 + hexLength; k++)
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

                    std::string hexColor = input.substr(i + 1, hexLength);

                    int colorValue = 0;
                    std::stringstream ss;
                    ss << std::hex << hexColor;
                    ss >> colorValue;

                    uint8_t red = (colorValue >> 16) & 0xFF;
                    uint8_t green = (colorValue >> 8) & 0xFF;
                    uint8_t blue = colorValue & 0xFF;
                    uint8_t alpha = (defaultColor >> 24) & 0xFF;

                    if (hexLength == 8)
                    {
                        alpha = colorValue & 0xFF;
                        blue = (colorValue >> 8) & 0xFF;
                        green = (colorValue >> 16) & 0xFF;
                        red = (colorValue >> 24) & 0xFF;
                    }
                    
                    currentColor = D3DCOLOR_RGBA(red, green, blue, alpha);

                    i += 1 + hexLength + 1;
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