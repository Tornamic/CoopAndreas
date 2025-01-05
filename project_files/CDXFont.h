#pragma once

struct CTextSegment
{
	std::string text;
	D3DCOLOR color;
};

class CDXFont
{
public:
	static ID3DXFont* m_pD3DXFont;
	static uint8_t m_fFontSize;

	static void Init();
	static void Draw(int x, int y, const char* text, D3DCOLOR color);
	static void DrawSegmentWithShadow(int x, int y, const char* text, D3DCOLOR color);
	static int GetSegmentWidth(const std::string& seg);
	static std::vector<CTextSegment> GetSegments(const std::string& input, D3DCOLOR defaultColor);

private:
	static void InitFont();
	static void DestroyFont();
};