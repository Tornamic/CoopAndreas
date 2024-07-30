#pragma once
class CDXFont
{
public:
	static void Init();

private:
	static void InitFont();
	static ID3DXFont* m_pD3DXFont;

	static void DestroyFont();
	static void Draw();
};