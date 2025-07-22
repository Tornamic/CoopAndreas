#if !defined(_COOPSERVER_VECTOR_VECTOR_H_) || !defined(_COOPSERVER_CVECTOR_H_)
#define _COOPSERVER_VECTOR_VECTOR_H_
#define _COOPSERVER_CVECTOR_H_

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

#endif