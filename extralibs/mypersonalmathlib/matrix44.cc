#include "matrix44.h"

//------------------------------------------------------------------------------
/**
*/
Matrix44::Matrix44(void)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			matrix[i][j] = 0;
		}
	}

	//Set matrix values to identity
	matrix[0][0] = 1;
	matrix[1][1] = 1;
	matrix[2][2] = 1;
	matrix[3][3] = 1;
}

//------------------------------------------------------------------------------
/**
*/
Matrix44::~Matrix44(void)
{
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::SetRow( unsigned int row, Vector4 v )
{
	try{
		if(row < 0 || row > 3)
		{	
			throw invalid_argument( "Received negative value or larger than 3" );
		}
	}
	catch(invalid_argument){
		cout<<"Invalid argument for SetRow"<<endl;
	}
	matrix[row][0] = v[0];
	matrix[row][1] = v[1];
	matrix[row][2] = v[2];
	matrix[row][3] = v[3];
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::SetColumn( unsigned int column, Vector4 v )
{
	try{
		if(column < 0 || column > 3)
		{	
			throw invalid_argument( "Received negative value or larger than 3" );
		}
	}
	catch(invalid_argument){
		cout<<"Invalid argument for SetColumn"<<endl;
	}
	matrix[0][column] = v[0];
	matrix[1][column] = v[1];
	matrix[2][column] = v[2];
	matrix[3][column] = v[3];
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::SetRow( unsigned int row, float x, float y, float z, float w )
{
	try{
		if(row < 0 || row > 3)
		{	
			throw invalid_argument( "Received negative value or larger than 3" );
		}
	}
	catch(invalid_argument){
		cout<<"Invalid argument for SetRow"<<endl;
	}
	matrix[row][0] = x;
	matrix[row][1] = y;
	matrix[row][2] = z;
	matrix[row][3] = w;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::SetColumn( unsigned int column, float x, float y, float z, float w )
{
	try{
		if(column < 0 || column > 3)
		{	
			throw invalid_argument( "Received negative value or larger than 3" );
		}
	}
	catch(invalid_argument){
		cout<<"Invalid argument for SetColumn"<<endl;
	}
	matrix[0][column] = x;
	matrix[1][column] = y;
	matrix[2][column] = z;
	matrix[3][column] = w;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::SetToIdentity()
{
	//Reset the values to 0
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			matrix[i][j] = 0;	
		}
	}

	//Set the matrix to an identity matrix
	matrix[0][0] = 1;
	matrix[1][1] = 1;
	matrix[2][2] = 1;
	matrix[3][3] = 1;
}

//------------------------------------------------------------------------------
/**
*/
ostream& operator<<(ostream &out, const Matrix44& m)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			out<<m.GetElement(i, j);
			out<<" ";
		}
		out<<"\n";
	}
	return out;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::operator*=(const Matrix44& m)
{
	//Temporary save the left matrix
	Matrix44 temp = *this;

	//Reset the values of the left matrix to 0
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			matrix[i][j] = 0;	
		}
	}

	//Multiplies each component of the left matrix row with each component of the right matrix column
	//Each multiplication is added to the index of the result matrix.
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				matrix[i][j] = matrix[i][j] + temp[i][k] * m.GetElement(k, j);
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
Matrix44 Matrix44::operator*(const Matrix44& m)const
{
	Matrix44 result;

	//Fill result matrix with 0
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i][j] = 0;	
		}
	}

	//Multiplies each component of the left matrix row with each component of the right matrix column
	//Each multiplication is added to the index of the result matrix.
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				result[i][j] = result[i][j] + matrix[i][k] * m.GetElement(k, j);
			}
		}
	}	
	return result;
}

//------------------------------------------------------------------------------
/**
*/
Vector4 Matrix44::operator*(const Vector4& v)const
{
	//Initialized with x,y,z,w = 0
	Vector4 result;

	//Multiplies each component of the left matrix row with each component of the right vector column
	//Each multiplication is added to the index of the result vector.
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i] = result[i] + matrix[i][j] * v.GetElement(j);
		}
	}	
	return result;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::operator*=(const float& number)
{
	Matrix44 temp = *this;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			matrix[i][j] = temp[i][j] * number;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
Matrix44 Matrix44::operator*(const float& number)const
{
	Matrix44 result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i][j] = matrix[i][j] * number;
		}
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
Matrix44 operator*(const float& number, const Matrix44& m)
{
	Matrix44 result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i][j] = number * m.GetElement(i, j);
		}
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
bool Matrix44::operator==(const Matrix44& m)const
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if(matrix[i][j] != m.GetElement(i, j)){
				return false;
			}
		}
	}
	return true;
}

//------------------------------------------------------------------------------
/**
*/
bool Matrix44::operator!=(const Matrix44& m)const
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (matrix[i][j] != m.GetElement(i, j)){
				return true;
			}
		}
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::Transpose()
{
	Matrix44 temp = *this;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			matrix[i][j] = temp[j][i];
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
float Matrix44::Determinant()
{
	return +matrix[0][0]*matrix[1][1]*matrix[2][2]*matrix[3][3]
			+matrix[0][0]*matrix[1][2]*matrix[2][3]*matrix[3][1]
			+matrix[0][0]*matrix[1][3]*matrix[2][1]*matrix[3][2]

			+matrix[0][1]*matrix[1][0]*matrix[2][3]*matrix[3][2]
			+matrix[0][1]*matrix[1][2]*matrix[2][0]*matrix[3][3]
			+matrix[0][1]*matrix[1][3]*matrix[2][2]*matrix[3][0]

			+matrix[0][2]*matrix[1][0]*matrix[2][1]*matrix[3][3]
			+matrix[0][2]*matrix[1][1]*matrix[2][3]*matrix[3][0]
			+matrix[0][2]*matrix[1][3]*matrix[2][0]*matrix[3][1]

			
			+matrix[0][3]*matrix[1][0]*matrix[2][2]*matrix[3][1]
			+matrix[0][3]*matrix[1][1]*matrix[2][0]*matrix[3][2]
			+matrix[0][3]*matrix[1][2]*matrix[2][1]*matrix[3][0]

			
			-matrix[0][0]*matrix[1][1]*matrix[2][3]*matrix[3][2]
			-matrix[0][0]*matrix[1][2]*matrix[2][1]*matrix[3][3]
			-matrix[0][0]*matrix[1][3]*matrix[2][2]*matrix[3][1]

			-matrix[0][1]*matrix[1][0]*matrix[2][2]*matrix[3][3]
			-matrix[0][1]*matrix[1][2]*matrix[2][3]*matrix[3][0]
			-matrix[0][1]*matrix[1][3]*matrix[2][0]*matrix[3][2]

			-matrix[0][2]*matrix[1][0]*matrix[2][3]*matrix[3][1]
			-matrix[0][2]*matrix[1][1]*matrix[2][0]*matrix[3][3]
			-matrix[0][2]*matrix[1][3]*matrix[2][1]*matrix[3][0]

			-matrix[0][3]*matrix[1][0]*matrix[2][1]*matrix[3][2]
			-matrix[0][3]*matrix[1][1]*matrix[2][2]*matrix[3][0]
			-matrix[0][3]*matrix[1][2]*matrix[2][0]*matrix[3][1];

}

//------------------------------------------------------------------------------
/**
*/
Matrix44 Matrix44::Inverse()
{
	Matrix44 result = *this;
	float det = result.Determinant();
	try{
		if(det == 0)
		{	
			throw domain_error("Determinant = 0, no Inverse.");
		}
	}
	catch(domain_error){
		cout<<"Determinant = 0, no Inverse for this matrix"<<endl;
		Matrix44 identityMatrix;
		return identityMatrix;
	}
	result[0][0] =  (matrix[1][1]*matrix[2][2]*matrix[3][3]
					+matrix[1][2]*matrix[2][3]*matrix[3][1]
					+matrix[1][3]*matrix[2][1]*matrix[3][2]
					-matrix[1][1]*matrix[2][3]*matrix[3][2]
					-matrix[1][2]*matrix[2][1]*matrix[3][3]
					-matrix[1][3]*matrix[2][2]*matrix[3][1]);

	result[0][1] =  (matrix[0][1]*matrix[2][3]*matrix[3][2]
					+matrix[0][2]*matrix[2][1]*matrix[3][3]
					+matrix[0][3]*matrix[2][2]*matrix[3][1]
					-matrix[0][1]*matrix[2][2]*matrix[3][3]
					-matrix[0][2]*matrix[2][3]*matrix[3][1]
					-matrix[0][3]*matrix[2][1]*matrix[3][2]);

	result[0][2] =  (matrix[0][1]*matrix[1][2]*matrix[3][3]
					+matrix[0][2]*matrix[1][3]*matrix[3][1]
					+matrix[0][3]*matrix[1][1]*matrix[3][2]
					-matrix[0][1]*matrix[1][3]*matrix[3][2]
					-matrix[0][2]*matrix[1][1]*matrix[3][3]
					-matrix[0][3]*matrix[1][2]*matrix[3][1]);

	result[0][3] =  (matrix[0][1]*matrix[1][3]*matrix[2][2]
					+matrix[0][2]*matrix[1][1]*matrix[2][3]
					+matrix[0][3]*matrix[1][2]*matrix[2][1]
					-matrix[0][1]*matrix[1][2]*matrix[2][3]
					-matrix[0][2]*matrix[1][3]*matrix[2][1]
					-matrix[0][3]*matrix[1][1]*matrix[2][2]);

	result[1][0] =  (matrix[1][0]*matrix[2][3]*matrix[3][2]
					+matrix[1][2]*matrix[2][0]*matrix[3][3]
					+matrix[1][3]*matrix[2][2]*matrix[3][0]
					-matrix[1][0]*matrix[2][2]*matrix[3][3]
					-matrix[1][2]*matrix[2][3]*matrix[3][0]
					-matrix[1][3]*matrix[2][0]*matrix[3][2]);

	result[1][1] =  (matrix[0][0]*matrix[2][2]*matrix[3][3]
					+matrix[0][2]*matrix[2][3]*matrix[3][0]
					+matrix[0][3]*matrix[2][0]*matrix[3][2]
					-matrix[0][0]*matrix[2][3]*matrix[3][2]
					-matrix[0][2]*matrix[2][0]*matrix[3][3]
					-matrix[0][3]*matrix[2][2]*matrix[3][0]);

	result[1][2] =  (matrix[0][0]*matrix[1][3]*matrix[3][2]
					+matrix[0][2]*matrix[1][0]*matrix[3][3]
					+matrix[0][3]*matrix[1][2]*matrix[3][0]
					-matrix[0][0]*matrix[1][2]*matrix[3][3]
					-matrix[0][2]*matrix[1][3]*matrix[3][0]
					-matrix[0][3]*matrix[1][0]*matrix[3][2]);

	result[1][3] =  (matrix[0][0]*matrix[1][2]*matrix[2][3]
					+matrix[0][2]*matrix[1][3]*matrix[2][0]
					+matrix[0][3]*matrix[1][0]*matrix[2][2]
					-matrix[0][0]*matrix[1][3]*matrix[2][2]
					-matrix[0][2]*matrix[1][0]*matrix[2][3]
					-matrix[0][3]*matrix[1][2]*matrix[2][0]);

	result[2][0] =  (matrix[1][0]*matrix[2][1]*matrix[3][3]
					+matrix[1][1]*matrix[2][3]*matrix[3][0]
					+matrix[1][3]*matrix[2][0]*matrix[3][1]
					-matrix[1][0]*matrix[2][3]*matrix[3][1]
					-matrix[1][1]*matrix[2][0]*matrix[3][3]
					-matrix[1][3]*matrix[2][1]*matrix[3][0]);

	result[2][1] =  (matrix[0][0]*matrix[2][3]*matrix[3][1]
					+matrix[0][1]*matrix[2][0]*matrix[3][3]
					+matrix[0][3]*matrix[2][1]*matrix[3][0]
					-matrix[0][0]*matrix[2][1]*matrix[3][3]
					-matrix[0][1]*matrix[2][3]*matrix[3][0]
					-matrix[0][3]*matrix[2][0]*matrix[3][1]);

	result[2][2] =  (matrix[0][0]*matrix[1][1]*matrix[3][3]
					+matrix[0][1]*matrix[1][3]*matrix[3][0]
					+matrix[0][3]*matrix[1][0]*matrix[3][1]
					-matrix[0][0]*matrix[1][3]*matrix[3][1]
					-matrix[0][1]*matrix[1][0]*matrix[3][3]
					-matrix[0][3]*matrix[1][1]*matrix[3][0]);

	result[2][3] =  (matrix[0][0]*matrix[1][3]*matrix[2][1]
					+matrix[0][1]*matrix[1][0]*matrix[2][3]
					+matrix[0][3]*matrix[1][1]*matrix[2][0]
					-matrix[0][0]*matrix[1][1]*matrix[2][3]
					-matrix[0][1]*matrix[1][3]*matrix[2][0]
					-matrix[0][3]*matrix[1][0]*matrix[2][1]);

	result[3][0] =  (matrix[1][0]*matrix[2][2]*matrix[3][1]
					+matrix[1][1]*matrix[2][0]*matrix[3][2]
					+matrix[1][2]*matrix[2][1]*matrix[3][0]
					-matrix[1][0]*matrix[2][1]*matrix[3][2]
					-matrix[1][1]*matrix[2][2]*matrix[3][0]
					-matrix[1][2]*matrix[2][0]*matrix[3][1]);

	result[3][1] =  (matrix[0][0]*matrix[2][1]*matrix[3][2]
					+matrix[0][1]*matrix[2][2]*matrix[3][0]
					+matrix[0][2]*matrix[2][0]*matrix[3][1]
					-matrix[0][0]*matrix[2][2]*matrix[3][1]
					-matrix[0][1]*matrix[2][0]*matrix[3][2]
					-matrix[0][2]*matrix[2][1]*matrix[3][0]);

	result[3][2] =  (matrix[0][0]*matrix[1][2]*matrix[3][1]
					+matrix[0][1]*matrix[1][0]*matrix[3][2]
					+matrix[0][2]*matrix[1][1]*matrix[3][0]
					-matrix[0][0]*matrix[1][1]*matrix[3][2]
					-matrix[0][1]*matrix[1][2]*matrix[3][0]
					-matrix[0][2]*matrix[1][0]*matrix[3][1]);

	result[3][3] =	(matrix[0][0]*matrix[1][1]*matrix[2][2]
					+matrix[0][1]*matrix[1][2]*matrix[2][0]
					+matrix[0][2]*matrix[1][0]*matrix[2][1]
					-matrix[0][0]*matrix[1][2]*matrix[2][1]
					-matrix[0][1]*matrix[1][0]*matrix[2][2]
					-matrix[0][2]*matrix[1][1]*matrix[2][0]);
	result = (1/det)*result;

	return result;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::RotateAroundX(float angle)
{
	Matrix44 rotationMatrix;
	float index00 = 1;
	float index01 = 0;
	float index02 = 0;
	float index03 = 0;

	float index10 = 0;
	float index11 = cosf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index12 = -sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index13 = 0;

	float index20 = 0;
	float index21 = sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index22 = cosf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index23 = 0;

	float index30 = 0;
	float index31 = 0;
	float index32 = 0;
	float index33 = 1;

	rotationMatrix.SetRow(0, Vector4(index00,index01,index02,index03));
	rotationMatrix.SetRow(1, Vector4(index10,index11,index12,index13));
	rotationMatrix.SetRow(2, Vector4(index20,index21,index22,index23));
	rotationMatrix.SetRow(3, Vector4(index30,index31,index32,index33));
	*this*=rotationMatrix;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::RotateAroundY(float angle)
{
	Matrix44 rotationMatrix;
	float index00 = cosf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index01 = 0;
	float index02 = sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index03 = 0;

	float index10 = 0;
	float index11 = 1;
	float index12 = 0;
	float index13 = 0;

	float index20 = -sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index21 = 0;
	float index22 = cosf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index23 = 0;

	float index30 = 0;
	float index31 = 0;
	float index32 = 0;
	float index33 = 1;

	rotationMatrix.SetRow(0, Vector4(index00,index01,index02,index03));
	rotationMatrix.SetRow(1, Vector4(index10,index11,index12,index13));
	rotationMatrix.SetRow(2, Vector4(index20,index21,index22,index23));
	rotationMatrix.SetRow(3, Vector4(index30,index31,index32,index33));
	*this*=rotationMatrix;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::RotateAroundZ(float angle)
{
	Matrix44 rotationMatrix;
	float index00 = cosf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index01 = -sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index02 = 0;
	float index03 = 0;

	float index10 = sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index11 = cosf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index12 = 0;
	float index13 = 0;

	float index20 = 0;
	float index21 = 0;
	float index22 = 1;
	float index23 = 0;

	float index30 = 0;
	float index31 = 0;
	float index32 = 0;
	float index33 = 1;

	rotationMatrix.SetRow(0, Vector4(index00,index01,index02,index03));
	rotationMatrix.SetRow(1, Vector4(index10,index11,index12,index13));
	rotationMatrix.SetRow(2, Vector4(index20,index21,index22,index23));
	rotationMatrix.SetRow(3, Vector4(index30,index31,index32,index33));
	*this*=rotationMatrix;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::Rotate(float angle, int x, int y, int z)
{
	Matrix44 rotationMatrix;
	Vector3 inputVector(x, y, z);
	inputVector.Normalize();
	float index00 = cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f) + (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*pow(inputVector[0],2);
	float index01 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[0]*inputVector[1]-inputVector[2]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index02 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[0]*inputVector[2]+inputVector[1]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index03 = 0;

	float index10 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[0]*inputVector[1]+inputVector[2]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index11 = cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f) + (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*pow(inputVector[1],2);
	float index12 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[1]*inputVector[2]-inputVector[0]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index13 = 0;

	float index20 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[0]*inputVector[2]-inputVector[1]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index21 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[1]*inputVector[2]+inputVector[0]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index22 = cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f) + (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*pow(inputVector[2],2);
	float index23 = 0;

	float index30 = 0;
	float index31 = 0;
	float index32 = 0;
	float index33 = 1;

	rotationMatrix.SetRow(0, Vector4(index00,index01,index02,index03));
	rotationMatrix.SetRow(1, Vector4(index10,index11,index12,index13));
	rotationMatrix.SetRow(2, Vector4(index20,index21,index22,index23));
	rotationMatrix.SetRow(3, Vector4(index30,index31,index32,index33));
	*this*=rotationMatrix;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::Rotate( float angle, Vector3 v )
{
	Matrix44 rotationMatrix;
	Vector3 inputVector = v;
	inputVector.Normalize();
	float index00 = cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f) + (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*pow(inputVector[0],2);
	float index01 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[0]*inputVector[1]-inputVector[2]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index02 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[0]*inputVector[2]+inputVector[1]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index03 = 0;

	float index10 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[0]*inputVector[1]+inputVector[2]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index11 = cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f) + (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*pow(inputVector[1],2);
	float index12 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[1]*inputVector[2]-inputVector[0]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index13 = 0;

	float index20 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[0]*inputVector[2]-inputVector[1]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index21 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[1]*inputVector[2]+inputVector[0]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index22 = cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f) + (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*pow(inputVector[2],2);
	float index23 = 0;

	float index30 = 0;
	float index31 = 0;
	float index32 = 0;
	float index33 = 1;

	rotationMatrix.SetRow(0, Vector4(index00,index01,index02,index03));
	rotationMatrix.SetRow(1, Vector4(index10,index11,index12,index13));
	rotationMatrix.SetRow(2, Vector4(index20,index21,index22,index23));
	rotationMatrix.SetRow(3, Vector4(index30,index31,index32,index33));
	*this*=rotationMatrix;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::Translate( float x, float y, float z )
{
	Matrix44 translationMatrix;
	float index00 = 1;
	float index01 = 0;
	float index02 = 0;
	float index03 = x;

	float index10 = 0;
	float index11 = 1;
	float index12 = 0;
	float index13 = y;

	float index20 = 0;
	float index21 = 0;
	float index22 = 1;
	float index23 = z;

	float index30 = 0;
	float index31 = 0;
	float index32 = 0;
	float index33 = 1;

	translationMatrix.SetRow(0, Vector4(index00,index01,index02,index03));
	translationMatrix.SetRow(1, Vector4(index10,index11,index12,index13));
	translationMatrix.SetRow(2, Vector4(index20,index21,index22,index23));
	translationMatrix.SetRow(3, Vector4(index30,index31,index32,index33));
	*this*=translationMatrix;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::Translate( Vector3 v )
{
	Matrix44 translationMatrix;
	float index00 = 1;
	float index01 = 0;
	float index02 = 0;
	float index03 = v[0];

	float index10 = 0;
	float index11 = 1;
	float index12 = 0;
	float index13 = v[1];

	float index20 = 0;
	float index21 = 0;
	float index22 = 1;
	float index23 = v[2];

	float index30 = 0;
	float index31 = 0;
	float index32 = 0;
	float index33 = 1;

	translationMatrix.SetRow(0, Vector4(index00,index01,index02,index03));
	translationMatrix.SetRow(1, Vector4(index10,index11,index12,index13));
	translationMatrix.SetRow(2, Vector4(index20,index21,index22,index23));
	translationMatrix.SetRow(3, Vector4(index30,index31,index32,index33));
	*this*=translationMatrix;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::Scale( float factor )
{
	Matrix44 scalingMatrix;
	float index00 = factor;
	float index01 = 0;
	float index02 = 0;
	float index03 = 0;

	float index10 = 0;
	float index11 = factor;
	float index12 = 0;
	float index13 = 0;

	float index20 = 0;
	float index21 = 0;
	float index22 = factor;
	float index23 = 0;

	float index30 = 0;
	float index31 = 0;
	float index32 = 0;
	float index33 = 1;

	scalingMatrix.SetRow(0, Vector4(index00,index01,index02,index03));
	scalingMatrix.SetRow(1, Vector4(index10,index11,index12,index13));
	scalingMatrix.SetRow(2, Vector4(index20,index21,index22,index23));
	scalingMatrix.SetRow(3, Vector4(index30,index31,index32,index33));
	*this*=scalingMatrix;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::Scale( float x, float y, float z )
{
	Matrix44 scalingMatrix;
	float index00 = x;
	float index01 = 0;
	float index02 = 0;
	float index03 = 0;

	float index10 = 0;
	float index11 = y;
	float index12 = 0;
	float index13 = 0;

	float index20 = 0;
	float index21 = 0;
	float index22 = z;
	float index23 = 0;

	float index30 = 0;
	float index31 = 0;
	float index32 = 0;
	float index33 = 1;

	scalingMatrix.SetRow(0, Vector4(index00,index01,index02,index03));
	scalingMatrix.SetRow(1, Vector4(index10,index11,index12,index13));
	scalingMatrix.SetRow(2, Vector4(index20,index21,index22,index23));
	scalingMatrix.SetRow(3, Vector4(index30,index31,index32,index33));
	*this*=scalingMatrix;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::Scale( Vector3 v )
{
	Matrix44 scalingMatrix;
	float index00 = v[0];
	float index01 = 0;
	float index02 = 0;
	float index03 = 0;

	float index10 = 0;
	float index11 = v[1];
	float index12 = 0;
	float index13 = 0;

	float index20 = 0;
	float index21 = 0;
	float index22 = v[2];
	float index23 = 0;

	float index30 = 0;
	float index31 = 0;
	float index32 = 0;
	float index33 = 1;

	scalingMatrix.SetRow(0, Vector4(index00,index01,index02,index03));
	scalingMatrix.SetRow(1, Vector4(index10,index11,index12,index13));
	scalingMatrix.SetRow(2, Vector4(index20,index21,index22,index23));
	scalingMatrix.SetRow(3, Vector4(index30,index31,index32,index33));
	*this*=scalingMatrix;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::LookAt(Vector3 eyePosition, Vector3 eyeTarget, Vector3 eyeUp)
{
	Vector3 forward = eyeTarget - eyePosition;
	forward.Normalize();

	Vector3 right = forward.Cross(eyeUp);
	right.Normalize();

	Vector3 up = right.Cross(forward);
	up.Normalize();

	Matrix44 rotation;
	Matrix44 translation;

	rotation[0][0] = right[0];
	rotation[0][1] = right[1];
	rotation[0][2] = right[2];
	rotation[0][3] = 0;

	rotation[1][0] = up[0];
	rotation[1][1] = up[1];
	rotation[1][2] = up[2];
	rotation[1][3] = 0;

	rotation[2][0] = -forward[0];
	rotation[2][1] = -forward[1];
	rotation[2][2] = -forward[2];
	rotation[2][3] = 0;

	rotation[3][0] = 0;
	rotation[3][1] = 0;
	rotation[3][2] = 0;
	rotation[3][3] = 1;

	translation.Translate(-eyePosition[0], -eyePosition[1], -eyePosition[2]);
	Matrix44 look_At;

	look_At = rotation*translation;
	*this = look_At;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::Perspective(float angle, float aspect, float near, float far)
{
	float f;
	//cot = cosf(x)/sinf(x) or 1/tan(x)
	f = 1/tan((angle/2)*MyPersonalMathLibraryConstants::PI/180.0f); //cosf(angle/2)/sinf(angle/2); 
	matrix[0][0] = f/aspect;
	matrix[0][1] = 0;
	matrix[0][2] = 0;
	matrix[0][3] = 0;

	matrix[1][0] = 0;
	matrix[1][1] = f;
	matrix[1][2] = 0;
	matrix[1][3] = 0;

	matrix[2][0] = 0;
	matrix[2][1] = 0;
	matrix[2][2] = (far+near)/(near-far);
	matrix[2][3] = (2*far*near)/(near-far);

	matrix[3][0] = 0;
	matrix[3][1] = 0;
	matrix[3][2] = -1;
	matrix[3][3] = 0;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::Ortho()
{
	matrix[0][0] = 1;
	matrix[0][1] = 0;
	matrix[0][2] = 0;
	matrix[0][3] = 0;

	matrix[1][0] = 0;
	matrix[1][1] = 1;
	matrix[1][2] = 0;
	matrix[1][3] = 0;

	matrix[2][0] = 0;
	matrix[2][1] = 0;
	matrix[2][2] = 0;
	matrix[2][3] = 0;

	matrix[3][0] = 0;
	matrix[3][1] = 0;
	matrix[3][2] = 0;
	matrix[3][3] = 1;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::Ortho( float left, float right, float bottom, float top, float near, float far ) //-aspect, aspect, -1, 1, 0, 100
{
	matrix[0][0] = 2/(right-left);
	matrix[0][1] = 0;
	matrix[0][2] = 0;
	matrix[0][3] = -((right+left)/(right-left));

	matrix[1][0] = 0;
	matrix[1][1] = 2/(top-bottom);
	matrix[1][2] = 0;
	matrix[1][3] = -((top+bottom)/(top-bottom));

	matrix[2][0] = 0;
	matrix[2][1] = 0;
	matrix[2][2] = -2/(far-near);
	matrix[2][3] = -((far+near)/(far-near));

	matrix[3][0] = 0;
	matrix[3][1] = 0;
	matrix[3][2] = 0;
	matrix[3][3] = 1;
}

//------------------------------------------------------------------------------
/**
*/
float* Matrix44::operator[](unsigned int index)
{
	return matrix[index];
}

float Matrix44::GetElement( unsigned int row, unsigned int column ) const
{
	return matrix[row][column];
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::ConvertToOpenGLArray(float matrixGL[16])
{
	//openGL handles column matrix.
	//float* result = new float[16];
	Matrix44 temp = *this;
	unsigned int k = 0;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			matrixGL[k] = temp[j][i]; //put columns into the float array
			k++;
		}
	}
	//return result;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix44::ConvertToFloatMatrix(float m[4][4])
{
	//openGL handles column matrix.
	Matrix44 temp = *this;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			m[i][j] = temp[j][i]; //put columns into the float array
		}
	}
}


