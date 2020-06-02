#include "Point4D.h"
#include <assert.h>
eae6320::Collision::Point4D::Point4D()
{
	col1 = 0.0f;
	col2 = 0.0f;
	col3 = 0.0f;
	col4 = 1.0f;
}

eae6320::Collision::Point4D::Point4D(float col_1, float col_2, float col_3, float col_4)
{
	col1 = col_1;
	col2 = col_2;
	col3 = col_3;
	col4 = col_4;
}

eae6320::Collision::Point4D::Point4D(float col_1, float col_2, float col_3)
{
	col1 = col_1;
	col2 = col_2;
	col3 = col_3;
	col4 = 1.0f;
}

eae6320::Collision::Point4D::~Point4D()
{
}

float& eae6320::Collision::Point4D::operator[](int i_index)
{
	assert(i_index < 4);
	assert(i_index >= 0);

	switch (i_index)
	{
	case 0:
		return col1;
		break;
	case 1:
		return col2;
		break;
	case 2:
		return col3;
		break;
	case 3:
		return col4;
		break;
	default:
		return col4;
		break;
	}
}
