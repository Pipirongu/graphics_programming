#include "vector4.h"

//------------------------------------------------------------------------------
/**
*/
Vector4::Vector4(float w)
{
	for (int i = 0; i < 4; i++){
		vector[i] = 0;
	}
	vector[3] = w;
}

//------------------------------------------------------------------------------
/**
*/
Vector4::Vector4( float x, float y, float z, float w )
{
	vector[0] = x;
	vector[1] = y;
	vector[2] = z;
	vector[3] = w;
}

//------------------------------------------------------------------------------
/**
*/
Vector4::~Vector4(void)
{
}

//------------------------------------------------------------------------------
/**
*/
void Vector4::Insert( float x, float y, float z, float w )
{
	vector[0] = x;
	vector[1] = y;
	vector[2] = z;
	vector[3] = w;
}

//------------------------------------------------------------------------------
/**
*/
void Vector4::InsertAt( unsigned int index, float value )
{
	vector[index] = value;
}

//------------------------------------------------------------------------------
/**
*/
ostream& operator<<(ostream &out, const Vector4& v)	
{
	for (int i = 0; i < 4; i++)
	{
		out<<v.GetElement(i)<<"\n";
	}
	return out;
}

//------------------------------------------------------------------------------
/**
*/
void Vector4::operator+=(const Vector4& v)
{
	Vector4 temp = *this;
	for (int i = 0; i < 4; i++)
	{
		vector[i] = temp[i] + v.GetElement(i);
	}
}

//------------------------------------------------------------------------------
/**
*/
Vector4 Vector4::operator+(const Vector4& v)const
{
	Vector4 result;
	for (int i = 0; i < 4; i++){
		result[i] = vector[i] + v.GetElement(i);
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
void Vector4::operator-=(const Vector4& v)
{
	Vector4 temp = *this;
	for (int i = 0; i < 4; i++)
	{
		vector[i] = temp[i] - v.GetElement(i);
	}
}

//------------------------------------------------------------------------------
/**
*/
Vector4 Vector4::operator-(const Vector4& v)const
{
	Vector4 result;
	for (int i = 0; i < 4; i++){
		result[i] = vector[i] - v.GetElement(i);
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
void Vector4::operator*=(const float& number)
{
	Vector4 temp = *this;
	for (int i = 0; i < 4; i++)
	{
		vector[i] =  temp[i] * number;
	}
}

//------------------------------------------------------------------------------
/**
*/
Vector4 Vector4::operator*(const float& number)const
{
	Vector4 result;
	for (int i = 0; i < 4; i++){
		result[i] = vector[i] * number;
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
void Vector4::operator/=(const float& number)
{
	Vector4 temp = *this;
	for (int i = 0; i < 4; i++)
	{
		vector[i] = temp[i] / number;
	}
}

//------------------------------------------------------------------------------
/**
*/
Vector4 Vector4::operator/(const float& number)const
{
	Vector4 result;
	for (int i = 0; i < 4; i++){
		result[i] = vector[i] / number;
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
Vector4 operator*(const float& number, const Vector4& v)
{
	Vector4 result;
	for (int i = 0; i < 4; i++)
	{
		result[i] = number * v.GetElement(i);
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
bool Vector4::operator==(const Vector4& v)const
{
	for (int i = 0; i < 4; i++)
	{
		if(vector[i] != v.GetElement(i)){
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------
/**
Multiply respective components of the vectors and add it to result
*/
float Vector4::Dot(const Vector4& v)
{
	float result = 0;
	for (int i = 0; i < 4; i++)
	{
		result = result + vector[i] * v.GetElement(i);
	}
	return result;
}

//------------------------------------------------------------------------------
/**
sqrt(X^2 + Y^2 + Z^2 + W^2)
*/
float Vector4::Magnitude()
{
	float x = vector[0];
	float y = vector[1];
	float z = vector[2];
	float w = vector[3];
	return sqrt(x*x + y*y + z*z + w*w);
}

//------------------------------------------------------------------------------
/**
*/
void Vector4::Normalize()
{	
	float length = Magnitude();
	Vector4 temp = *this;
	for (int i = 0; i < 4; i++)
	{
		vector[i] = temp[i]/length;
	}
}

//------------------------------------------------------------------------------
/**
*/
Vector4 Vector4::Normalized()
{
	Vector4 result;
	float length = Magnitude();
	for (int i = 0; i < 4; i++)
	{
		result[i] = vector[i]/length;
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
float& Vector4::operator[](unsigned int index)
{
	return vector[index];
}

//------------------------------------------------------------------------------
/**
*/
float Vector4::GetElement( unsigned int index ) const
{
	return vector[index];
}
