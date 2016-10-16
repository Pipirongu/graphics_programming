#ifndef matrix44_h__
#define matrix44_h__

#include "vector4.h"
#include "vector3.h"
#include "mypersonalmathlibconstants.h"

#include <iostream>
#include <stdexcept>
#include <math.h> 
using namespace std;

/**
@class Matrix44

A Matrix44 class
*/
class Matrix44
{
public:
	Matrix44(void);
	~Matrix44(void);

	/// Sets a Row of elements in the Matrix
	void SetRow(unsigned int row, Vector4 v);
	/// Sets a Column of elements in the Matrix
	void SetColumn(unsigned int column, Vector4 v);
	/// Sets a Row of elements in the Matrix
	void SetRow(unsigned int row, float x, float y, float z, float w);
	/// Sets a Column of elements in the Matrix
	void SetColumn(unsigned int column, float x, float y, float z, float w);

	/// Sets this matrix to an Identity Matrix
	void SetToIdentity();

	/// Overloaded operator to print out a Matrix
	friend ostream& operator<<(ostream &out, const Matrix44& m);

	/// Matrix44*=Matrix44. Multiplication is affecting the left Matrix
	void operator*=(const Matrix44& m);
	/// Matrix44*Matrix44
	Matrix44 operator*(const Matrix44& m)const;

	/// Matrix44*Vector4
	Vector4 operator*(const Vector4& v)const;

	/// Matrix44*=number. Multiplication is affecting the Matrix
	void operator*=(const float& number);
	/// Matrix44*number
	Matrix44 operator*(const float& number)const;

	/// number*Matrix44
	friend Matrix44 operator*(const float& number, const Matrix44& m);

	/// Compares two Matrices if they are the same
	bool operator==(const Matrix44& m)const;

	/// Compares two Matrices if they are different
	bool operator!=(const Matrix44& m)const;

	/// Transpose the Matrix, switching rows to columns
	void Transpose();
	/// Returns the Determinant of a matrix
	float Determinant();
	/// Returns the Inverse of a Matrix. Only possible if (Determinant != 0). Returns the Identity Matrix if Determinant = 0
	Matrix44 Inverse();

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

	/// Multiply the matrix with a Translation matrix.
	void Translate(float x, float y, float z);
	/// Multiply the matrix with a Translation matrix.
	void Translate(Vector3 v);

	/// Multiply the matrix with a Scaling matrix. Scales equally for xyz axis
	void Scale(float factor);
	/// Multiply the matrix with a Scaling matrix.
	void Scale(float x, float y, float z);
	/// Multiply the matrix with a Scaling matrix.
	void Scale (Vector3 v);

	/// lookAt function meant for camera
	void LookAt(Vector3 eyePosition, Vector3 eyeTarget, Vector3 eyeUp);

	/// Sets the matrix to a Perspective projection matrix
	void Perspective(float angle, float aspect, float near, float far);
	/// Sets the matrix to an Orthographic projection matrix
	void Ortho();
	/// Sets the matrix to an OpenGL Orthographic projection matrix
	void Ortho(float left, float right, float bottom, float top, float near, float far);

	/// Overloaded access operator for a Matrix
	float* operator[](unsigned int x);
	/// Returns element at specified index
	float GetElement(unsigned int row, unsigned int column) const;

	/// Takes a 16 float array as argument which is filled with the matrix data
	void ConvertToOpenGLArray(float matrixGL[16]);
	/// Takes a 4x4 float array as argument which is filled with the matrix data
	void ConvertToFloatMatrix(float m[4][4]);

private:
	//Matrix 4x4
	float matrix[4][4];
};
#endif // matrix44_h__