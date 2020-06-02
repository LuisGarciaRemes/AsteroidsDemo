#include "Matrix4x4.h"
#include <assert.h>

eae6320::Collision::Matrix4x4::Matrix4x4()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			matrix[i][j] = 0.0f;
		}
	}
}

eae6320::Collision::Matrix4x4::Matrix4x4(eae6320::Collision::Point4D row1, eae6320::Collision::Point4D row2, eae6320::Collision::Point4D row3, eae6320::Collision::Point4D row4)
{
	matrix[0] = row1;
	matrix[1] = row2;
	matrix[2] = row3;
	matrix[3] = row4;
}

const eae6320::Collision::Matrix4x4 eae6320::Collision::Matrix4x4::Transpose()
{
	eae6320::Collision::Matrix4x4 tempMatrix;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			tempMatrix[j][i] = matrix[i][j];
		}
	}
	return tempMatrix;
}

const eae6320::Collision::Matrix4x4 eae6320::Collision::Matrix4x4::Adjugate()
{
	eae6320::Collision::Matrix4x4 tempMatrix;

	tempMatrix[0][0] = ((matrix[1][1] * matrix[2][2] * matrix[3][3]) + (matrix[1][2] * matrix[2][3] * matrix[3][1]) + (matrix[1][3] * matrix[2][1] * matrix[3][2])
		- (matrix[1][3] * matrix[2][2] * matrix[3][1]) - (matrix[1][2] * matrix[2][1] * matrix[3][3]) - (matrix[1][1] * matrix[2][3] * matrix[3][2]));

	tempMatrix[0][1] = (-(matrix[0][1] * matrix[2][2] * matrix[3][3]) - (matrix[0][2] * matrix[2][3] * matrix[3][1]) - (matrix[0][3] * matrix[2][1] * matrix[3][2])
		+ (matrix[0][3] * matrix[2][2] * matrix[3][1]) + (matrix[0][2] * matrix[2][1] * matrix[3][3]) + (matrix[0][1] * matrix[2][3] * matrix[3][2]));

	tempMatrix[0][2] = ((matrix[0][1] * matrix[1][2] * matrix[3][3]) + (matrix[0][2] * matrix[1][3] * matrix[3][1]) + (matrix[0][3] * matrix[1][1] * matrix[3][2])
		- (matrix[0][3] * matrix[1][2] * matrix[3][1]) - (matrix[0][2] * matrix[1][1] * matrix[3][3]) - (matrix[0][1] * matrix[1][3] * matrix[3][2]));

	tempMatrix[0][3] = (-(matrix[0][1] * matrix[1][2] * matrix[2][3]) - (matrix[0][2] * matrix[1][3] * matrix[2][1]) - (matrix[0][3] * matrix[1][1] * matrix[2][2])
		+ (matrix[0][3] * matrix[1][2] * matrix[2][1]) + (matrix[0][2] * matrix[1][1] * matrix[2][3]) + (matrix[0][1] * matrix[1][3] * matrix[2][2]));



	tempMatrix[1][0] = (-(matrix[1][0] * matrix[2][2] * matrix[3][3]) - (matrix[1][2] * matrix[2][3] * matrix[3][0]) - (matrix[1][3] * matrix[2][0] * matrix[3][2])
		+ (matrix[1][3] * matrix[2][2] * matrix[3][0]) + (matrix[1][2] * matrix[2][0] * matrix[3][3]) + (matrix[1][0] * matrix[2][3] * matrix[3][2]));

	tempMatrix[1][1] = ((matrix[0][0] * matrix[2][2] * matrix[3][3]) + (matrix[0][2] * matrix[2][3] * matrix[3][0]) + (matrix[0][3] * matrix[2][0] * matrix[3][2])
		- (matrix[0][3] * matrix[2][2] * matrix[3][0]) - (matrix[0][2] * matrix[2][0] * matrix[3][3]) - (matrix[0][0] * matrix[2][3] * matrix[3][2]));

	tempMatrix[1][2] = (-(matrix[0][0] * matrix[1][2] * matrix[3][3]) - (matrix[0][2] * matrix[1][3] * matrix[3][0]) - (matrix[0][3] * matrix[1][0] * matrix[3][2])
		+ (matrix[0][3] * matrix[1][2] * matrix[3][0]) + (matrix[0][2] * matrix[1][0] * matrix[3][3]) + (matrix[0][0] * matrix[1][3] * matrix[3][2]));

	tempMatrix[1][3] = ((matrix[0][0] * matrix[1][2] * matrix[2][3]) + (matrix[0][2] * matrix[1][3] * matrix[2][0]) + (matrix[0][3] * matrix[1][0] * matrix[2][2])
		- (matrix[0][3] * matrix[1][2] * matrix[2][0]) - (matrix[0][2] * matrix[1][0] * matrix[2][3]) - (matrix[0][0] * matrix[1][3] * matrix[2][2]));


	tempMatrix[2][0] = ((matrix[1][0] * matrix[2][1] * matrix[3][3]) + (matrix[1][1] * matrix[2][3] * matrix[3][0]) + (matrix[1][3] * matrix[2][0] * matrix[3][1])
		- (matrix[1][3] * matrix[2][2] * matrix[3][1]) - (matrix[1][2] * matrix[2][1] * matrix[3][3]) - (matrix[1][1] * matrix[2][3] * matrix[3][1]));

	tempMatrix[2][1] = (-(matrix[0][0] * matrix[2][1] * matrix[3][3]) - (matrix[0][1] * matrix[2][3] * matrix[3][0]) - (matrix[0][3] * matrix[2][0] * matrix[3][1])
		+ (matrix[0][3] * matrix[2][1] * matrix[3][0]) + (matrix[0][1] * matrix[2][0] * matrix[3][3]) + (matrix[0][0] * matrix[2][3] * matrix[3][1]));

	tempMatrix[2][2] = ((matrix[0][0] * matrix[1][1] * matrix[3][3]) + (matrix[0][1] * matrix[1][3] * matrix[3][0]) + (matrix[0][3] * matrix[1][0] * matrix[3][1])
		- (matrix[0][3] * matrix[1][1] * matrix[3][0]) - (matrix[0][1] * matrix[1][0] * matrix[3][3]) - (matrix[0][0] * matrix[1][3] * matrix[3][1]));

	tempMatrix[2][3] = (-(matrix[0][0] * matrix[1][1] * matrix[2][3]) - (matrix[0][1] * matrix[1][3] * matrix[2][0]) - (matrix[0][3] * matrix[1][0] * matrix[2][1])
		+ (matrix[0][3] * matrix[1][1] * matrix[2][0]) + (matrix[0][1] * matrix[1][0] * matrix[2][3]) + (matrix[0][0] * matrix[1][3] * matrix[2][1]));


	tempMatrix[3][0] = (-(matrix[1][0] * matrix[2][1] * matrix[3][2]) - (matrix[1][1] * matrix[2][2] * matrix[3][0]) - (matrix[1][2] * matrix[2][0] * matrix[3][1])
		+ (matrix[1][2] * matrix[2][1] * matrix[3][0]) + (matrix[1][1] * matrix[2][0] * matrix[3][2]) + (matrix[1][0] * matrix[2][2] * matrix[3][1]));

	tempMatrix[3][1] = ((matrix[0][0] * matrix[2][1] * matrix[3][2]) + (matrix[0][1] * matrix[2][2] * matrix[3][0]) + (matrix[0][2] * matrix[2][0] * matrix[3][1])
		- (matrix[0][2] * matrix[2][1] * matrix[3][0]) - (matrix[0][1] * matrix[2][0] * matrix[3][2]) - (matrix[0][0] * matrix[2][2] * matrix[3][1]));

	tempMatrix[3][2] = (-(matrix[0][0] * matrix[1][1] * matrix[3][2]) - (matrix[0][1] * matrix[1][2] * matrix[3][0]) - (matrix[0][2] * matrix[1][0] * matrix[3][1])
		+ (matrix[0][2] * matrix[1][1] * matrix[3][0]) + (matrix[0][1] * matrix[1][0] * matrix[3][2]) + (matrix[0][0] * matrix[1][2] * matrix[3][1]));

	tempMatrix[3][3] = ((matrix[0][0] * matrix[1][1] * matrix[2][2]) + (matrix[0][1] * matrix[1][2] * matrix[2][0]) + (matrix[0][2] * matrix[1][0] * matrix[2][1])
		- (matrix[0][2] * matrix[1][1] * matrix[2][0]) - (matrix[0][1] * matrix[1][0] * matrix[2][2]) - (matrix[0][0] * matrix[1][2] * matrix[2][1]));

	return tempMatrix;
}

const eae6320::Collision::Matrix4x4 eae6320::Collision::Matrix4x4::Inverse()
{
	eae6320::Collision::Matrix4x4 temp = eae6320::Collision::Matrix4x4(matrix[0], matrix[1], matrix[2], matrix[3]);

	float tempfloat = 0.0f;

	assert(temp.Determinant() != 0.0f);

	tempfloat = 1.0f / temp.Determinant();


	temp = temp.Adjugate();

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			temp[i][j] = temp[i][j] * tempfloat;
		}
	}
	return temp;
}

const eae6320::Collision::Matrix4x4 eae6320::Collision::Matrix4x4::operator*(eae6320::Collision::Matrix4x4& other)
{
	eae6320::Collision::Matrix4x4 tempMatrix;
	float tempVal;
	for (int k = 0; k < 4; k++)
	{
		for (int i = 0; i < 4; i++)
		{
			tempVal = 0.0f;
			for (int j = 0; j < 4; j++)
			{
				tempVal += matrix[i][j] * other[j][k];
			}
			tempMatrix[i][k] = tempVal;
		}
	}
	return tempMatrix;
}

const eae6320::Collision::Matrix4x4 eae6320::Collision::Matrix4x4::operator+(eae6320::Collision::Matrix4x4& other)
{
	eae6320::Collision::Matrix4x4 tempMatrix;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			tempMatrix[i][j] = matrix[i][j] + other[i][j];
		}
	}
	return tempMatrix;
}

const eae6320::Collision::Matrix4x4 eae6320::Collision::Matrix4x4::operator-(eae6320::Collision::Matrix4x4& other)
{
	eae6320::Collision::Matrix4x4 tempMatrix;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			tempMatrix[i][j] = matrix[i][j] - other[i][j];
		}
	}
	return tempMatrix;
}

eae6320::Collision::Point4D eae6320::Collision::Matrix4x4::operator*(eae6320::Collision::Point4D& other)
{
	eae6320::Collision::Point4D tempPoint4D;
	float tempVal;

	for (int i = 0; i < 4; i++)
	{
		tempVal = 0.0f;
		for (int j = 0; j < 4; j++)
		{
			tempVal += matrix[i][j] * other[j];
		}
		tempPoint4D[i] = tempVal;
	}
	return tempPoint4D;
}

eae6320::Collision::Point4D& eae6320::Collision::Matrix4x4::operator[](int i_index)
{
	assert(i_index < 4);
	assert(i_index >= 0);
	return matrix[i_index];
}

eae6320::Collision::Matrix4x4::~Matrix4x4()
{
}
