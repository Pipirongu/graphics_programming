#ifndef vector3_h__
#define vector3_h__

#include <iostream>
#include <math.h> 
using namespace std;

/**
@class Vector3

Vector3 class
*/
class Vector3
{
public:
	Vector3(void);
	/// A Vector of size 3 with x, y, z values
	Vector3(float x, float y, float z);
	~Vector3(void);

	/// Set x, y, z values of the Vector
	void Insert(float x, float y, float z);
	/// Set a value of a component of a vector
	void InsertAt(unsigned int index, float value);

	/// Overloaded operator to print out a Vector
	friend ostream& operator<<(ostream &out, const Vector3& v);

	/// Vector3+=Vector3. Addition is affecting the left Vector
	void operator+=(const Vector3& v);
	/// Vector3+Vector3
	Vector3 operator+(const Vector3& v)const; 

	/// Vector3-=Vector3. Substraction is affecting the left Vector
	void operator-=(const Vector3& v);
	/// Vector3-Vector3
	Vector3 operator-(const Vector3& v)const;

	/// Vector3*=number. Multiplication is affecting the left Vector
	void operator*=(const float& number);
	/// Vector3*number
	Vector3 operator*(const float& number)const;

	/// Vector3/=number. Division is affecting the left Vector
	void operator/=(const float& number);
	/// Vector3/number
	Vector3 operator/(const float& number)const;

	/// number*Vector3
	friend Vector3 operator*(const float& number, const Vector3& v);

	/// Compares two vectors if they are the same
	bool operator==(const Vector3& v)const;

	/// Compares two vectors if they are different
	bool operator!=(const Vector3& v)const;

	/// Returns the dot product of this vector
	float Dot(const Vector3& v);
	/// Returns the length of this vector
	float Magnitude();
	/// Normalize this vector
	void Normalize();
	/// Return the normalized vector
	Vector3 Normalized();

	/// Cross Product between two Vectors
	Vector3 Cross(const Vector3& v);

	/// Overloaded access operator for Vector
	float& operator[](unsigned int index);
	/// Returns element at specified index
	float GetElement(unsigned int index) const;

private:
	float vector[3];

};
#endif // vector3_h__