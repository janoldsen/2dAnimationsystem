
#ifndef MY_MATH_H
#define MY_MATH_H

#include <math.h>

#define MIN(a, b) (a<b?a:b)
#define MAX(a, b) (a>b?a:b)

#define PI 3.14159265358979323846

struct float2
{
	float x;
	float y;
};


struct int2
{
	int x;
	int y;
};

struct int3
{
	int x;
	int y;
	int z;
};


struct intRect
{
	int x;
	int y;
	int width;
	int height;
};



struct floatRect
{
	union
	{
		struct
		{
			float x;
			float y;
		};

		float2 pos;
	};

	union
	{
		struct
		{
			float width;
			float height;
		};

		float2 extend;
	};
};

struct float2x2
{
	float m00;
	float m01;
	float m10;
	float m11;
};

struct float3x3
{
	float m00;
	float m01;
	float m02;

	float m10;
	float m11;
	float m12;

	float m20;
	float m21;
	float m22;

};


inline float2 rotate(const float2& v, float angle)
{
	return{ v.x * cos(angle) + v.y * sin(angle), v.x * -sin(angle) + v.y *  cos(angle) };
}



inline float2 operator*(const float2& v, float f)
{
	return{ v.x * f, v.y * f};
}


inline float2 operator*(const float2& v1, const float2& v2)
{
	return{ v1.x * v2.x, v1.y * v2.y };
}

inline float2 operator*(const float2& v, const float2x2& mat)
{
	return{ v.x * mat.m00 + v.y * mat.m01, v.x * mat.m10 + v.y * mat.m11 };
}

inline float2 operator*(const float2& v, const float3x3& mat)
{
	return{ v.x * mat.m00 + v.y * mat.m01 + mat.m02, v.x * mat.m10 + v.y * mat.m11 + mat.m12};
}

inline float2 operator+(const float2& lhs, const float2& rhs)
{
	return{ lhs.x + rhs.x, lhs.y + rhs.y };
}

inline float2& operator+=(float2& lhs, const float2& rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

inline bool operator==(const float2& lhs, const float2& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}


inline bool operator!=(const float2& lhs, const float2& rhs)
{
	return !(lhs == rhs);
}

inline float2 operator-(const float2& lhs, const float2& rhs)
{
	return{ lhs.x - rhs.x, lhs.y - rhs.y };
}


inline float2x2 createRotationMatrix(float angle)
{
	return{ cos(angle), sin(angle), -sin(angle), cos(angle) };
}




inline int2 operator+(const int2& lhs, const int2& rhs)
{
	return{ lhs.x + rhs.x, lhs.y + rhs.y };
}



inline bool operator==(const int3& lhs, const int3& rhs)
{
	return (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z);
}


inline float3x3 createTransformationMatrix(float2 position, float2 scale, float angle)
{
	return float3x3{

		cos(angle) * scale.x, sin(angle) * scale.x, 0,
		-sin(angle) * scale.y, cos(angle) * scale.y, 0,
		position.x, position.y, 1
	};
}


inline bool pointInRect(int2 point, intRect rect)
{
	return point.x >= rect.x && point.x < rect.x + rect.width &&
		point.y >= rect.y && point.y < rect.y + rect.height;
}

inline bool pointInRect(float2 point, floatRect rect)
{
	return point.x >= rect.x && point.x < rect.x + rect.width &&
		point.y >= rect.y && point.y < rect.y + rect.height;
}

inline bool pointInRect(float2 point, intRect rect)
{
	return point.x >= rect.x && point.x < rect.x + rect.width &&
		point.y >= rect.y && point.y < rect.y + rect.height;
}



inline float length(float2 v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}

inline float dot(float2 a, float2 b)
{
	return a.x * b.x + a.y * b.y;
}

inline float2 normalize(float2 v)
{
	float l = length(v);
	return{ v.x / l, v.y / l };
}


inline float correctAngle(float angle)
{
	//return fmod((double)angle, PI);

	return fmod(angle > 0 ? angle : 2.0f * PI + angle, 2.0f * PI);
}

inline float2 slerp(const float2& v0, const float2& v1, float t)
{
	if (v0 == v1)
		return v0;

	float theta0 = acos(MAX(MIN(dot(v0, v1), 1.0f), -1.0f));
	float theta = theta0 * t;

	float2 v2 = normalize(v1 - v0 * dot(v0, v1));

	return v0*cos(theta) + v2*sin(theta);
}


inline float2 getRotation(const float2& v0, const float2& v1)
{

	float2 _v0 = normalize(v0);
	float2 _v1 = normalize(v1);

	return{ _v0.x * _v1.x + _v0.y * _v1.y, _v0.y * _v1.x - _v0.x * _v1.y };
}

inline float2 rotate(const float2& v, const float2& r)
{
	return{ v.x * r.x - v.y * r.y, v.x * r.y + v.y * r.x };
}

inline float2 negateRotation(const float2& v)
{
	return{ v.x, -v.y };
}

inline float interpolate(float a, float b, float t)
{
	return a*(1 - t) + b*t;
}

inline float2 interpolate(float2 a, float2 b, float t)
{
	return a*(1 - t) + b*t;
}


#endif

