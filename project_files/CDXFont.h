#pragma once
class CDXFont
{
public:
	static void Init();
	static ID3DXFont* m_pD3DXFont;
	static void Draw(int x, int y, const char* text, D3DCOLOR color);

private:
	static void InitFont();

	static void DestroyFont();
};