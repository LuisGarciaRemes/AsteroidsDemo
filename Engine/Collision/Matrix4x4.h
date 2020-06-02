#pragma once
#include "Point4D.h"
#include <math.h>

#define DegreesToRadians 0.0174533

namespace eae6320
{
	namespace Collision
	{
		class Matrix4x4
		{
		public:
			//constructors
			Matrix4x4();
			Matrix4x4(eae6320::Collision::Point4D row1, eae6320::Collision::Point4D row2,eae6320::Collision:: Point4D row3, eae6320::Collision::Point4D row4);

			//Matrix operations
			const Matrix4x4 Transpose();
			const Matrix4x4 Adjugate();
			const Matrix4x4 Inverse();
			inline const float Determinant()
			{
				return matrix[0][0] * ((matrix[1][1] * matrix[2][2] * matrix[3][3]) + (matrix[1][2] * matrix[2][3] * matrix[3][1]) + (matrix[1][3] * matrix[2][1] * matrix[3][2]) - (matrix[1][3] * matrix[2][2] * matrix[3][1]) - (matrix[1][2] * matrix[2][1] * matrix[3][3]) - (matrix[1][1] * matrix[2][3] * matrix[3][2]))
					- matrix[1][0] * ((matrix[0][1] * matrix[2][2] * matrix[3][3]) + (matrix[0][2] * matrix[2][3] * matrix[3][1]) + (matrix[0][3] * matrix[2][1] * matrix[3][2]) - (matrix[0][3] * matrix[2][2] * matrix[3][1]) - (matrix[0][2] * matrix[2][1] * matrix[3][3]) - (matrix[0][1] * matrix[2][3] * matrix[3][2]))
					+ matrix[2][0] * ((matrix[0][1] * matrix[1][2] * matrix[3][3]) + (matrix[0][2] * matrix[1][3] * matrix[3][1]) + (matrix[0][3] * matrix[1][1] * matrix[3][2]) - (matrix[0][3] * matrix[1][2] * matrix[3][1]) - (matrix[0][2] * matrix[1][1] * matrix[3][3]) - (matrix[0][1] * matrix[1][3] * matrix[3][2]))
					- matrix[3][0] * ((matrix[0][1] * matrix[1][2] * matrix[2][3]) + (matrix[0][2] * matrix[1][3] * matrix[2][1]) + (matrix[0][3] * matrix[1][1] * matrix[2][2]) - (matrix[0][3] * matrix[1][2] * matrix[2][1]) - (matrix[0][2] * matrix[1][1] * matrix[2][3]) - (matrix[0][1] * matrix[1][3] * matrix[2][2]));
			}

			//operators
			const Matrix4x4 operator* (Matrix4x4& other);
			const Matrix4x4 operator+ (Matrix4x4& other);
			const Matrix4x4 operator- (Matrix4x4& other);
			eae6320::Collision::Point4D operator* (Point4D& other);
			eae6320::Collision::Point4D& operator[](int i_index);

			
			//Usefull Transforms
			static Matrix4x4 ZAxisRotationTransform(float angle)
			{
				return Matrix4x4(eae6320::Collision::Point4D((float) cos(angle * DegreesToRadians), (float) -sin(angle * DegreesToRadians), 0.0f, 0.0f), eae6320::Collision::Point4D((float) sin(angle * DegreesToRadians), (float) cos(angle * DegreesToRadians), 0.0f, 0.0f), eae6320::Collision::Point4D(0.0f, 0.0f, 1.0f, 0.0f), eae6320::Collision::Point4D(0.0f, 0.0f, 0.0f, 1.0f));
			}

			static Matrix4x4 YAxisRotationTransform(float angle)
			{
				return Matrix4x4(eae6320::Collision::Point4D((float) cos(angle * DegreesToRadians), 0.0f, (float) sin(angle * DegreesToRadians), 0.0f), eae6320::Collision::Point4D(0.0f, 1.0f, 0.0f, 0.0f), eae6320::Collision::Point4D((float) -sin(angle * DegreesToRadians), 0.0f, (float) cos(angle * DegreesToRadians), 0.0f), eae6320::Collision::Point4D(0.0f, 0.0f, 0.0f, 1.0f));
			}

			static Matrix4x4 XAxisRotationTransform(float angle)
			{
				return Matrix4x4(eae6320::Collision::Point4D(1.0f, 0.0f, 0.0f, 0.0f), eae6320::Collision::Point4D(0.0f, (float) cos(angle * DegreesToRadians), (float) -sin(angle * DegreesToRadians), 0.0f), eae6320::Collision::Point4D(0.0f, (float) sin(angle * DegreesToRadians), (float) cos(angle * DegreesToRadians), 0.0f), eae6320::Collision::Point4D(0.0f, 0.0f, 0.0f, 1.0f));
			}

			static Matrix4x4 TranslationTransform(float x, float y, float z)
			{
				return Matrix4x4(eae6320::Collision::Point4D(1.0f, 0.0f, 0.0f, x), eae6320::Collision::Point4D(0.0f, 1.0f, 0.0f, y), eae6320::Collision::Point4D(0.0f, 0.0f, 1.0f, z), eae6320::Collision::Point4D(0.0f, 0.0f, 0.0f, 1.0f));
			}

			static Matrix4x4 ScalingTransform(float x, float y, float z)
			{
				return Matrix4x4(eae6320::Collision::Point4D(x, 0.0f, 0.0f, 0.0f), eae6320::Collision::Point4D(0.0f, y, 0.0f, 0.0f), eae6320::Collision::Point4D(0.0f, 0.0f, z, 0.0f), eae6320::Collision::Point4D(0.0f, 0.0f, 0.0f, 1.0f));
			}

			~Matrix4x4();
		private:			
			eae6320::Collision::Point4D matrix[4];
		};
	}
}

