#ifndef matrix33_h__
#define matrix33_h__

#include "vector3.h"
#include "mypersonalmathlibconstants.h"

#include <iostream>
#include <stdexcept>
#include <math.h> 
using namespace std;

/**
@class Matrix33

A Matrix33 class
*/
class Matrix33
{
public:
	Matrix33(void);
	~Matrix33(void);

	/// Sets a Row of elements in the Matrix
	void SetRow(unsigned int row, Vector3 v);
	/// Sets a Column of elements in the Matrix
	void SetColumn(unsigned int column, Vector3 v);
	/// Sets a Row of elements in the Matrix
	void SetRow(unsigned int row, float x, float y, float z);
	/// Sets a Column of elements in the Matrix
	void SetColumn(unsigned int column, float x, float y, float z);
	
	/// Sets this matrix to an Identity Matrix
	void SetToIdentity();

	/// Overloaded operator to print out a Matrix
	friend ostream& operator<<(ostream &out, const Matrix33& m);

	/// Matrix33*=Matrix33. Multiplication is affecting the left Matrix
	void operator*=(const Matrix33& m);
	/// Matrix33*Matrix33
	Matrix33 operator*(const Matrix33& m)const;

	/// Matrix33*Vector3
	Vector3 operator*(const Vector3& v)const;

	/// Matrix33*=number. Multiplication is affecting the Matrix
	void operator*=(const float& number);
	/// Matrix33*number
	Matrix33 operator*(const float& number)const;

	/// number*Matrix
	friend Matrix33 operator*(const float& number, const Matrix33& m);

	/// Compares two Matrices if they are the same
	bool operator==(const Matrix33& m)const;
	
	/// Transpose this Matrix, switching rows to columns
	void Transpose();
	/// Returns the Determinant of a matrix
	float Determinant();
	/// Returns the Inverse of a Matrix. Only possible if (Determinant != 0). Returns the Identity Matrix if Determinant = 0
	Matrix33 Inverse();

	/// Multiply the matrix with a Rotation matrix around X axis
	void RotateAroundX(float angle);
	/// Multiply the matrix with a Rotation matrix around Y axis
	void RotateAroundY(float angle);
	/// Multiply the matrix with a Rotation matrix around Z axis
	void RotateAroundZ(float angle);

	/// Multiply the matrix with a Rotation matrix. Around an arbitary axis
	void Rotate(float angle, int x, int y, int z);
	/// Multiply the matrix with a Rotation matrix. Around an arbitary axis
	void Rotate(float angle, Vector3 v);
	

	/// Overloaded access operator for a Matrix
	float* operator[](unsigned int index);
	/// Returns element at specified index
	float GetElement(unsigned int row, unsigned int column) const;
	
private:
	//holds matrix44
	float matrix[4][4];
};
#endif // matrix33_h__
