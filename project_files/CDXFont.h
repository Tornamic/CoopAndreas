#pragma once

struct CTextSegment
{
	std::wstring text;
	D3DCOLOR color;
};

class CDXFont
{
public:
	static inline constexpr uint8_t m_iShadowSize = 1;

	static ID3DXFont* m_pD3DXFont;
	static uint8_t m_fFontSize;

	static void Init();
	static void Draw(int x, int y, const std::vector<CTextSegment>& segments);
	static void Draw(int x, int y, const std::string& str, D3DCOLOR defaultColor);
	static void Draw(int x, int y, const std::wstring& str, D3DCOLOR defaultColor);
	static void DrawSegmentWithShadow(int x, int y, const wchar_t* text, D3DCOLOR color);
	static int GetTextWidth(const std::wstring& seg);
	static std::vector<CTextSegment> ParseColorSegments(const std::wstring& input, D3DCOLOR defaultColor);

private:
	static void InitFont();
	static void DestroyFont();
};