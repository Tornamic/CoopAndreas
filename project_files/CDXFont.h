#pragma once
class CDXFont
{
public:
	static void Init();
	static ID3DXFont* m_pD3DXFont;

private:
	static void InitFont();

	static void DestroyFont();
	static void Draw();
};