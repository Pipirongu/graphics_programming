#include "matrix33.h"

//------------------------------------------------------------------------------
/**
*/
Matrix33::Matrix33(void)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			matrix[i][j] = 0;
		}
	}

	//Set matrix values to identity
	matrix[0][0] = 1;
	matrix[1][1] = 1;
	matrix[2][2] = 1;
}

//------------------------------------------------------------------------------
/**
*/
Matrix33::~Matrix33(void)
{
}

//------------------------------------------------------------------------------
/**
*/
void Matrix33::SetRow( unsigned int row, Vector3 v )
{
	try{
		if(row < 0 || row > 2)
		{	
			throw invalid_argument( "Received negative value or larger than 2" );
		}
	}
	catch(invalid_argument){
		cout<<"Invalid argument for SetRow"<<endl;
	}
	matrix[row][0] = v.GetElement(0);
	matrix[row][1] = v.GetElement(1);
	matrix[row][2] = v.GetElement(2);
}

//------------------------------------------------------------------------------
/**
*/
void Matrix33::SetColumn( unsigned int column, Vector3 v )
{
	try{
		if(column < 0 || column > 2)
		{	
			throw invalid_argument( "Received negative value or larger than 2" );
		}
	}
	catch(invalid_argument){
		cout<<"Invalid argument for SetColumn"<<endl;
	}
	matrix[0][column] = v.GetElement(0);
	matrix[1][column] = v.GetElement(1);
	matrix[2][column] = v.GetElement(2);
}

//------------------------------------------------------------------------------
/**
*/
void Matrix33::SetRow( unsigned int row, float x, float y, float z )
{
	try{
		if(row < 0 || row > 2)
		{	
			throw invalid_argument( "Received negative value or larger than 2" );
		}
	}
	catch(invalid_argument){
		cout<<"Invalid argument for SetRow"<<endl;
	}
	matrix[row][0] = x;
	matrix[row][1] = y;
	matrix[row][2] = z;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix33::SetColumn( unsigned int column, float x, float y, float z )
{
	try{
		if(column < 0 || column > 2)
		{	
			throw invalid_argument( "Received negative value or larger than 2" );
		}
	}
	catch(invalid_argument){
		cout<<"Invalid argument for SetColumn"<<endl;
	}
	matrix[0][column] = x;
	matrix[1][column] = y;
	matrix[2][column] = z;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix33::SetToIdentity()
{
	//Reset the values to 0
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			matrix[i][j] = 0;	
		}
	}

	//Set the matrix to an identity matrix
	matrix[0][0] = 1;
	matrix[1][1] = 1;
	matrix[2][2] = 1;
}

//------------------------------------------------------------------------------
/**
*/
ostream& operator<<(ostream &out, const Matrix33& m)	
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
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
void Matrix33::operator*=(const Matrix33& m)
{
	//Temporary save the left matrix
	Matrix33 temp = *this;

	//Reset the left matrix with 0
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			matrix[i][j] = 0;	
		}
	}

	//Multiplies each component of the left matrix row with each component of the right matrix column
	//Each multiplication is added to the index of the result matrix.
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				matrix[i][j] = matrix[i][j] + temp[i][k] * m.GetElement(k, j);
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
Matrix33 Matrix33::operator*(const Matrix33& m)const
{
	Matrix33 result;

	//Fill the result matrix with 0
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			result[i][j] = 0;	
		}
	}

	//Multiplies each component of the left matrix row with each component of the right matrix column
	//Each multiplication is added to the index of the result matrix.
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
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
Vector3 Matrix33::operator*(const Vector3& v)const
{
	//Initialized with x,y,z = 0
	Vector3 result;

	//Multiplies each component of the left matrix row with each component of the right vector column
	//Each multiplication is added to the index of the result vector.
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			result[i] = result[i] + matrix[i][j] * v.GetElement(j);
		}
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix33::operator*=(const float& number)
{
	Matrix33 temp = *this;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			matrix[i][j] = temp[i][j] * number;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
Matrix33 Matrix33::operator*(const float& number)const
{
	Matrix33 result;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			result[i][j] = matrix[i][j] * number;
		}
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
Matrix33 operator*(const float& number, const Matrix33& m)
{
	Matrix33 result;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			result[i][j] = number * m.GetElement(i, j);
		}
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
bool Matrix33::operator==(const Matrix33& m)const
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if(matrix[i][j] != m.GetElement(i, j)){;
				return false;
			}
		}
	}
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix33::Transpose()
{
	Matrix33 temp = *this;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			matrix[i][j] = temp[j][i];
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
float Matrix33::Determinant()
{
	return +matrix[0][0]*(matrix[1][1]*matrix[2][2] - matrix[1][2]*matrix[2][1])
		   -matrix[0][1]*(matrix[1][0]*matrix[2][2] - matrix[1][2]*matrix[2][0])
		   +matrix[0][2]*(matrix[1][0]*matrix[2][1] - matrix[1][1]*matrix[2][0]);
}

//------------------------------------------------------------------------------
/**
*/
Matrix33 Matrix33::Inverse()
{
	Matrix33 result = *this;
	float det = result.Determinant();
	try{
		if(det == 0)
		{	
			throw domain_error("Determinant = 0, no Inverse.");
		}
	}
	catch(domain_error){
		cout<<"Determinant = 0, no Inverse for this matrix"<<endl;
		Matrix33 identityMatrix;
		identityMatrix.SetToIdentity();
		return identityMatrix;
	}
	result[0][0] = (matrix[1][1]*matrix[2][2] - matrix[1][2]*matrix[2][1]);
	result[0][1] = (matrix[0][2]*matrix[2][1] - matrix[0][1]*matrix[2][2]);
	result[0][2] = (matrix[0][1]*matrix[1][2] - matrix[0][2]*matrix[1][1]);

	result[1][0] = (matrix[1][2]*matrix[2][0] - matrix[1][0]*matrix[2][2]);
	result[1][1] = (matrix[0][0]*matrix[2][2] - matrix[0][2]*matrix[2][0]);
	result[1][2] = (matrix[0][2]*matrix[1][0] - matrix[0][0]*matrix[1][2]);

	result[2][0] = (matrix[1][0]*matrix[2][1] - matrix[1][1]*matrix[2][0]);
	result[2][1] = (matrix[0][1]*matrix[2][0] - matrix[0][0]*matrix[2][1]);
	result[2][2] = (matrix[0][0]*matrix[1][1] - matrix[0][1]*matrix[1][0]);
	result = (1/det)*result;

	return result;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix33::RotateAroundX(float angle)
{
	Matrix33 rotationMatrix;
	float index00 = 1;
	float index01 = 0;
	float index02 = 0;

	float index10 = 0;
	float index11 = cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f);
	float index12 = -sinf(angle * MyPersonalMathLibraryConstants::PI/180.0f);

	float index20 = 0;
	float index21 = sinf(angle * MyPersonalMathLibraryConstants::PI/180.0f);
	float index22 = cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f);

	rotationMatrix.SetRow(0, Vector3(index00,index01,index02));
	rotationMatrix.SetRow(1, Vector3(index10,index11,index12));
	rotationMatrix.SetRow(2, Vector3(index20,index21,index22));
	*this*=rotationMatrix;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix33::RotateAroundY(float angle)
{
	Matrix33 rotationMatrix;
	float index00 = cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f);
	float index01 = 0;
	float index02 = sinf(angle * MyPersonalMathLibraryConstants::PI/180.0f);

	float index10 = 0;
	float index11 = 1;
	float index12 = 0;

	float index20 = -sinf(angle * MyPersonalMathLibraryConstants::PI/180.0f);
	float index21 = 0;
	float index22 = cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f);

	rotationMatrix.SetRow(0, Vector3(index00,index01,index02));
	rotationMatrix.SetRow(1, Vector3(index10,index11,index12));
	rotationMatrix.SetRow(2, Vector3(index20,index21,index22));
	*this*=rotationMatrix;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix33::RotateAroundZ(float angle)
{
	Matrix33 rotationMatrix;
	float index00 = cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f);
	float index01 = -sinf(angle * MyPersonalMathLibraryConstants::PI/180.0f);
	float index02 = 0;

	float index10 = sinf(angle * MyPersonalMathLibraryConstants::PI/180.0f);
	float index11 = cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f);
	float index12 = 0;

	float index20 = 0;
	float index21 = 0;
	float index22 = 1;

	rotationMatrix.SetRow(0, Vector3(index00,index01,index02));
	rotationMatrix.SetRow(1, Vector3(index10,index11,index12));
	rotationMatrix.SetRow(2, Vector3(index20,index21,index22));
	*this*=rotationMatrix;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix33::Rotate( float angle, int x, int y, int z )
{
	Matrix33 rotationMatrix;
	Vector3 inputVector(x, y, z);
	inputVector.Normalize();
	float index00 = cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f) + (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*pow(inputVector[0],2);
	float index01 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[0]*inputVector[1]-inputVector[2]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index02 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[0]*inputVector[2]+inputVector[1]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);

	float index10 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[0]*inputVector[1]+inputVector[2]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index11 = cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f) + (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*pow(inputVector[1],2);
	float index12 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[1]*inputVector[2]-inputVector[0]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);

	float index20 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[0]*inputVector[2]-inputVector[1]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index21 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[1]*inputVector[2]+inputVector[0]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index22 = cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f) + (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*pow(inputVector[2],2);

	rotationMatrix.SetRow(0, Vector3(index00,index01,index02));
	rotationMatrix.SetRow(1, Vector3(index10,index11,index12));
	rotationMatrix.SetRow(2, Vector3(index20,index21,index22));
	*this*=rotationMatrix;
}

//------------------------------------------------------------------------------
/**
*/
void Matrix33::Rotate(float angle, Vector3 v)
{
	Matrix33 rotationMatrix;
	Vector3 inputVector = v;
	inputVector.Normalize();
	float index00 = cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f) + (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*pow(inputVector[0],2);
	float index01 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[0]*inputVector[1]-inputVector[2]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index02 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[0]*inputVector[2]+inputVector[1]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);

	float index10 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[0]*inputVector[1]+inputVector[2]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index11 = cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f) + (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*pow(inputVector[1],2);
	float index12 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[1]*inputVector[2]-inputVector[0]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);

	float index20 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[0]*inputVector[2]-inputVector[1]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index21 = (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*inputVector[1]*inputVector[2]+inputVector[0]*sinf(angle*MyPersonalMathLibraryConstants::PI/180.0f);
	float index22 = cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f) + (1-cosf(angle * MyPersonalMathLibraryConstants::PI/180.0f))*pow(inputVector[2],2);

	rotationMatrix.SetRow(0, Vector3(index00,index01,index02));
	rotationMatrix.SetRow(1, Vector3(index10,index11,index12));
	rotationMatrix.SetRow(2, Vector3(index20,index21,index22));
	*this*=rotationMatrix;
}

//------------------------------------------------------------------------------
/**
*/
float* Matrix33::operator[](unsigned int index)
{
	return matrix[index];
}

//------------------------------------------------------------------------------
/**
*/
float Matrix33::GetElement( unsigned int row, unsigned int column ) const
{
	return matrix[row][column];
}
