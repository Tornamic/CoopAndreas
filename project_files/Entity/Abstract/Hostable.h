#pragma once

class Hostable
{
public:
	bool m_bHostedByMe = false;
	virtual ~Hostable() = default;
};