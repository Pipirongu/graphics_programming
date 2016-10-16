#include "vector3.h"

//------------------------------------------------------------------------------
/**
*/
Vector3::Vector3(void)
{
	for (int i = 0; i < 3; i++){
		vector[i] = 0;
	}
}

//------------------------------------------------------------------------------
/**
*/
Vector3::Vector3( float x, float y, float z )
{
	vector[0] = x;
	vector[1] = y;
	vector[2] = z;
}

//------------------------------------------------------------------------------
/**
*/
Vector3::~Vector3(void)
{
}

//------------------------------------------------------------------------------
/**
*/
void Vector3::Insert( float x, float y, float z )
{
	vector[0] = x;
	vector[1] = y;
	vector[2] = z;
}

//------------------------------------------------------------------------------
/**
*/
void Vector3::InsertAt( unsigned int index, float value )
{
	vector[index] = value;
}

//------------------------------------------------------------------------------
/**
*/
ostream& operator<<(ostream &out, const Vector3& v)	
{
	for (int i = 0; i < 3; i++)
	{
		out<<v.GetElement(i)<<"\n";
	}
	return out;
}

//------------------------------------------------------------------------------
/**
*/
void Vector3::operator+=(const Vector3& v)
{
	Vector3 temp = *this;
	for (int i = 0; i < 3; i++)
	{
		vector[i] = temp[i] + v.GetElement(i);
	}
}

//------------------------------------------------------------------------------
/**
*/
Vector3 Vector3::operator+(const Vector3& v)const
{
	Vector3 result;
	for (int i = 0; i < 3; i++){
		result[i] = vector[i] + v.GetElement(i);
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
void Vector3::operator-=(const Vector3& v)
{
	Vector3 temp = *this;
	for (int i = 0; i < 3; i++)
	{
		vector[i] = temp[i] - v.GetElement(i);
	}
}

//------------------------------------------------------------------------------
/**
*/
Vector3 Vector3::operator-(const Vector3& v)const
{
	Vector3 result;
	for (int i = 0; i < 3; i++){
		result[i] = vector[i] - v.GetElement(i);
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
void Vector3::operator*=(const float& number)
{
	Vector3 temp = *this;
	for (int i = 0; i < 3; i++)
	{
		vector[i] =  temp[i] * number;
	}
}

//------------------------------------------------------------------------------
/**
*/
Vector3 Vector3::operator*(const float& number)const
{
	Vector3 result;
	for (int i = 0; i < 3; i++){
		result[i] = vector[i] * number;
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
void Vector3::operator/=(const float& number)
{
	Vector3 temp = *this;
	for (int i = 0; i < 3; i++)
	{
		vector[i] = temp[i] / number;
	}
}

//------------------------------------------------------------------------------
/**
*/
Vector3 Vector3::operator/(const float& number)const
{
	Vector3 result;
	for (int i = 0; i < 3; i++){
		result[i] = vector[i] / number;
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
Vector3 operator*(const float& number, const Vector3& v)
{
	Vector3 result;
	for (int i = 0; i < 3; i++)
	{
		result[i] = number * v.GetElement(i);
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
bool Vector3::operator==(const Vector3& v)const
{
	for (int i = 0; i < 3; i++)
	{
		if(vector[i] != v.GetElement(i)){
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------
/**
*/
bool Vector3::operator!=(const Vector3& v)const
{
	for (int i = 0; i < 3; i++)
	{
		if (vector[i] != v.GetElement(i)){
			return true;
		}
	}
	return false;
}


//------------------------------------------------------------------------------
/**
Multiply respective components of the vectors and add it to result
*/
float Vector3::Dot(const Vector3& v)
{
	float result = 0;
	for (int i = 0; i < 3; i++)
	{
		result = result + vector[i] * v.GetElement(i);
	}
	return result;
}

//------------------------------------------------------------------------------
/**
sqrt(X^2 + Y^2 + Z^2)
*/
float Vector3::Magnitude()
{
	float x = vector[0];
	float y = vector[1];
	float z = vector[2];
	return sqrt(x*x + y*y + z*z);
}

//------------------------------------------------------------------------------
/**
*/
void Vector3::Normalize()
{	
	float length = Magnitude();
	Vector3 temp = *this;
	for (int i = 0; i < 3; i++)
	{
		vector[i] = temp[i]/length;
	}
}

//------------------------------------------------------------------------------
/**
*/
Vector3 Vector3::Normalized()
{
	Vector3 result;
	float length = Magnitude();
	for (int i = 0; i < 3; i++)
	{
		result[i] = vector[i]/length;
	}
	return result;
}

//------------------------------------------------------------------------------
/**
///Cross Product between two Vectors, using the "pencil" rule
*/
Vector3 Vector3::Cross(const Vector3& v)
{
	Vector3 result;
	result[0] = (vector[1]*v.GetElement(2)) - (vector[2]*v.GetElement(1));
	result[1] = -((vector[0]*v.GetElement(2)) - (vector[2]*v.GetElement(0)));
	result[2] = (vector[0]*v.GetElement(1)) - (vector[1]*v.GetElement(0));
	
	return result;
}

//------------------------------------------------------------------------------
/**
*/
float& Vector3::operator[](unsigned int index)
{
	return vector[index];
}

//------------------------------------------------------------------------------
/**
*/
float Vector3::GetElement( unsigned int index ) const
{
	return vector[index];
}
