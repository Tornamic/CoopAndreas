#pragma once
class CDXFont
{
public:
	static void InitFont();

private:
	static ID3DXFont* m_pD3DXFont;

	static void DestroyFont();
	static void Draw();
};