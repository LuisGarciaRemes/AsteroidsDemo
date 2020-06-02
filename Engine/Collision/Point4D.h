#pragma once

namespace eae6320
{
	namespace Collision
	{
		class Point4D
		{
		public:
			Point4D();
			Point4D(float col_1, float col_2, float col_3, float col_4);
			Point4D(float col_1, float col_2, float col_3);
			~Point4D();

			//get
			inline float Col1() const { return col1; }
			inline float Col2() const { return col2; }
			inline float Col3() const { return col3; }
			inline float Col4() const { return col4; }

			//set
			inline void Col1(float col_1) { col1 = col_1; }
			inline void Col2(float col_2) { col2 = col_2; }
			inline void Col3(float col_3) { col1 = col_3; }
			inline void Col4(float col_4) { col2 = col_4; }

			//Operators
			inline const Point4D operator+ (const Point4D& i_other) const
			{
				return Point4D(col1 + i_other.col1, col2 + i_other.col2, col3 + i_other.col3, col4 + i_other.col4);
			}

			inline const Point4D operator* (const float& i_other) const
			{
				return Point4D(col1 * i_other, col2 * i_other, col3 * i_other, col4 * i_other);
			}

			inline const Point4D operator/ (const float& i_other) const
			{
				return Point4D(col1 / i_other, col2 / i_other, col3 / i_other, col4 / i_other);
			}

			inline const Point4D operator- (const Point4D& i_other) const
			{
				return Point4D(col1 - i_other.col1, col2 - i_other.col2, col3 - i_other.col3, col4 - i_other.col4);
			}

			inline void operator-= (const Point4D& i_other)
			{
				col1 = col1 - i_other.col1, col2 = col2 - i_other.col2, col3 = col3 - i_other.col3, col4 = col4 - i_other.col4;
			}
			inline void operator+= (const Point4D& i_other)
			{
				col1 = col1 + i_other.col1, col2 = col2 + i_other.col2, col3 = col3 + i_other.col3, col4 = col4 + i_other.col4;
			}
			inline const bool operator== (const Point4D& i_other)
			{
				return (col1 == i_other.col1 && col2 == i_other.col2 && col3 == i_other.col3 && col4 == i_other.col4) ? true : false;
			}

			float& operator[](int i_index);

		private:
			float col1, col2, col3, col4;
		};
	}
}


