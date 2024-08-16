#pragma once
struct CVector
{
public:
	float x;
	float y;
	float z;

	CVector()
	{
		x = y = z = 0.0f;
	}

	CVector(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
};