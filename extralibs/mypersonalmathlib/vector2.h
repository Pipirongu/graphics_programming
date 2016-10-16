#ifndef vector2_h__
#define vector2_h__

#include <iostream>
#include <math.h> 
using namespace std;

/**
@class Vector2

Vector2 class
*/
class Vector2
{
public:
	Vector2(void);
	/// A Vector of size 2 with x, y values
	Vector2(float x, float y);
	~Vector2(void);

	/// Set x, y values of the Vector
	void Insert(float x, float y);
	/// Set a value of a component of a vector
	void InsertAt(unsigned int index, float value);

	/// Overloaded operator to print out a Vector
	friend ostream& operator<<(ostream &out, const Vector2& v);


	/// Vector2+=Vector2. Addition is affecting the left Vector
	void operator+=(const Vector2& v);
	/// Vector2+Vector2
	Vector2 operator+(const Vector2& v)const; 

	/// Vector2-=Vector2. Substraction is affecting the left Vector
	void operator-=(const Vector2& v);
	/// Vector2-Vector2
	Vector2 operator-(const Vector2& v)const;

	/// Vector2*=number. Multiplication is affecting the left Vector
	void operator*=(const float& number);
	/// Vector2*number
	Vector2 operator*(const float& number)const;

	/// Vector2/=number. Division is affecting the left Vector
	void operator/=(const float& number);
	/// Vector2/number
	Vector2 operator/(const float& number)const;

	/// number*Vector2
	friend Vector2 operator*(const float& number, const Vector2& v);

	/// Compares two vectors if they are the same
	bool operator==(const Vector2& v)const;

	/// Returns the dot product of this vector
	float Dot(const Vector2& v);
	/// Returns the length of this vector
	float Magnitude();
	/// Normalize this vector
	void Normalize();
	/// Return the normalized vector
	Vector2 Normalized();

	/// Overloaded access operator for Vector
	float& operator[](unsigned int index);
	/// Returns element at specified index
	float GetElement(unsigned int index) const;

private:
	float vector[2];
};

#endif // vector2_h__
