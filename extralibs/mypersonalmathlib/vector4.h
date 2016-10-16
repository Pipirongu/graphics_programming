#ifndef vector4_h__
#define vector4_h__

#include <iostream>
#include <math.h> 
using namespace std;

/**
@class Vector4

Vector4 class
*/
class Vector4
{
public:
	/// A Vector of size 4 is created with values 0, except w which can be set. Defaults to 1.
	Vector4(float w=1);
	/// A Vector of size 4 with x, y, z, w values. W defaults to 1.
	Vector4(float x, float y, float z, float w=1);
	~Vector4(void);

	/// Set x, y, z, w values of a Vector. W defaults to 1.
	void Insert(float x, float y, float z, float w=1);
	/// Set a value of a component of a vector
	void InsertAt(unsigned int index, float value);

	/// Overloaded operator to print out a Vector
	friend ostream& operator<<(ostream &out, const Vector4& v);

	/// Vector4+=Vector4. Addition is affecting the left Vector
	void operator+=(const Vector4& v);
	/// Vector4+Vector4
	Vector4 operator+(const Vector4& v)const; 

	/// Vector4-=Vector4. Substraction is affecting the left Vector
	void operator-=(const Vector4& v);
	/// Vector4-Vector4
	Vector4 operator-(const Vector4& v)const;

	/// Vector4*=number. Multiplication is affecting the left Vector
	void operator*=(const float& number);
	/// Vector4*number
	Vector4 operator*(const float& number)const;

	/// Vector4/=number. Division is affecting the left Vector
	void operator/=(const float& number);
	/// Vector4/number
	Vector4 operator/(const float& number)const;

	/// number*Vector4
	friend Vector4 operator*(const float& number, const Vector4& v);

	/// Compares two vectors if they are the same
	bool operator==(const Vector4& v)const;

	/// Returns the dot product of this vector
	float Dot(const Vector4& v);
	/// Returns the length of this vector
	float Magnitude();
	/// Normalize this vector
	void Normalize();
	/// Return the normalized vector
	Vector4 Normalized();

	/// Overloaded access operator for Vector
	float& operator[](unsigned int index);
	/// Returns element at specified index
	float GetElement(unsigned int index) const;

private:
	float vector[4];
};
#endif // vector4_h__