#include "vector2.h"

//------------------------------------------------------------------------------
/**
*/
Vector2::Vector2(void)
{
	for (int i = 0; i < 2; i++){
		vector[i] = 0;
	}
}

//------------------------------------------------------------------------------
/**
*/
Vector2::Vector2( float x, float y )
{
	vector[0] = x;
	vector[1] = y;
}

//------------------------------------------------------------------------------
/**
*/
Vector2::~Vector2(void)
{
}

//------------------------------------------------------------------------------
/**
*/
void Vector2::Insert( float x, float y )
{
	vector[0] = x;
	vector[1] = y;
}

//------------------------------------------------------------------------------
/**
*/
void Vector2::InsertAt( unsigned int index, float value )
{
	vector[index] = value;
}

//------------------------------------------------------------------------------
/**
*/
ostream& operator<<(ostream &out, const Vector2& v)	
{
	for (int i = 0; i < 2; i++)
	{
		out<<v.GetElement(i)<<"\n";
	}
	return out;
}

//------------------------------------------------------------------------------
/**
*/
void Vector2::operator+=(const Vector2& v)
{
	Vector2 temp = *this;
	for (int i = 0; i < 2; i++)
	{
		vector[i] = temp[i] + v.GetElement(i);
	}
}

//------------------------------------------------------------------------------
/**
*/
Vector2 Vector2::operator+(const Vector2& v)const
{
	Vector2 result;
	for (int i = 0; i < 2; i++){
		result[i] = vector[i] + v.GetElement(i);
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
void Vector2::operator-=(const Vector2& v)
{
	Vector2 temp = *this;
	for (int i = 0; i < 2; i++)
	{
		vector[i] = temp[i] - v.GetElement(i);
	}
}

//------------------------------------------------------------------------------
/**
*/
Vector2 Vector2::operator-(const Vector2& v)const
{
	Vector2 result;
	for (int i = 0; i < 2; i++){
		result[i] = vector[i] - v.GetElement(i);
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
void Vector2::operator*=(const float& number)
{
	Vector2 temp = *this;
	for (int i = 0; i < 2; i++)
	{
		vector[i] =  temp[i] * number;
	}
}

//------------------------------------------------------------------------------
/**
*/
Vector2 Vector2::operator*(const float& number)const
{
	Vector2 result;
	for (int i = 0; i < 2; i++){
		result[i] = vector[i] * number;
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
void Vector2::operator/=(const float& number)
{
	Vector2 temp = *this;
	for (int i = 0; i < 2; i++)
	{
		vector[i] = temp[i] / number;
	}
}

//------------------------------------------------------------------------------
/**
*/
Vector2 Vector2::operator/(const float& number)const
{
	Vector2 result;
	for (int i = 0; i < 2; i++){
		result[i] = vector[i] / number;
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
Vector2 operator*(const float& number, const Vector2& v)
{
	Vector2 result;
	for (int i = 0; i < 2; i++)
	{
		result[i] = number * v.GetElement(i);
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
bool Vector2::operator==(const Vector2& v)const
{
	for (int i = 0; i < 2; i++)
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
float Vector2::Dot(const Vector2& v)
{
	float result = 0;
	for (int i = 0; i < 2; i++)
	{
		result = result + vector[i] * v.GetElement(i);
	}
	return result;
}

//------------------------------------------------------------------------------
/**
sqrt(X^2 + Y^2)
*/
float Vector2::Magnitude()
{
	float x = vector[0];
	float y = vector[1];
	return sqrt(x*x + y*y);
}

//------------------------------------------------------------------------------
/**
*/
void Vector2::Normalize()
{	
	float length = Magnitude();
	Vector2 temp = *this;
	for (int i = 0; i < 2; i++)
	{
		vector[i] = temp[i]/length;
	}
}

//------------------------------------------------------------------------------
/**
*/
Vector2 Vector2::Normalized()
{
	Vector2 result;
	float length = Magnitude();
	for (int i = 0; i < 2; i++)
	{
		result[i] = vector[i]/length;
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
float& Vector2::operator[](unsigned int index)
{
	return vector[index];
}

//------------------------------------------------------------------------------
/**
*/
float Vector2::GetElement( unsigned int index ) const
{
	return vector[index];
}
