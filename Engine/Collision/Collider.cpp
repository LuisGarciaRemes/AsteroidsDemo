#include "Collider.h"
#include "Matrix4x4.h"
#include <Engine\Platform\Platform.h>

uint16_t currId = 0;

eae6320::cResult eae6320::Collision::Collider::InitializeCollider(float i_width, float i_height, uint16_t i_layer, uint16_t i_id)
{
	rect = new BoxCollider();
	x = 0.0f;
	y = 0.0f;
	(*rect).width = i_width;
	(*rect).height = i_height;
	layer = i_layer;
	id = i_id;	
	return eae6320::Results::Success;
}

eae6320::cResult eae6320::Collision::Collider::InitializeCollider(float i_radius, uint16_t i_layer, uint16_t i_id)
{
	circle = new CircleCollider();
	x = 0.0f;
	y = 0.0f;
	(*circle).radius = i_radius*2; //Not sure but I seem to have some issues where scale is not working properly unless I use the diameter
	layer = i_layer;
	id = i_id;
	return eae6320::Results::Success;
}

eae6320::cResult eae6320::Collision::Collider::FactoryHelper(eae6320::Collision::Collider*& o_Collider, float i_radius, uint16_t i_layer, uint16_t i_id)
{
	o_Collider = new Collider();
	return o_Collider->InitializeCollider(i_radius,i_layer,i_id);
}

eae6320::cResult eae6320::Collision::Collider::FactoryHelper(eae6320::Collision::Collider*& o_Collider,  float i_width, float i_height, uint16_t i_layer, uint16_t i_id)
{
	o_Collider = new Collider();
	return o_Collider->InitializeCollider(i_width,i_height, i_layer,i_id);
}

eae6320::Collision::Collider::Collider()
{
	rect = nullptr;
	circle = nullptr;
	layer = 0;
	x = 0.0f;
	y = 0.0f;
	id = 0;
}

eae6320::Collision::Collider::~Collider()
{
}

bool eae6320::Collision::Collider::CheckCollisions(eae6320::Collision::Collider* other)
{
	if (rect != nullptr && (*other).rect != nullptr)
	{
		return Check2RectCollision(other);
	}
	else if (circle != nullptr && (*other).circle != nullptr)
	{
		return Check2CircleCollision(other);
	}
	else if (circle == nullptr && (*other).circle != nullptr)
	{
		return other->CheckCircleRectCollision(this);
	}
	else if (circle != nullptr && (*other).circle == nullptr)
	{
		return CheckCircleRectCollision(other);
	}

	return false;
}

bool eae6320::Collision::Collider::Check2RectCollision(eae6320::Collision::Collider* other)
{
	eae6320::Collision::Matrix4x4 bTrans = eae6320::Collision::Matrix4x4::TranslationTransform((*other).x, (*other).y, 0.0f);
	eae6320::Collision::Matrix4x4 bRots = eae6320::Collision::Matrix4x4::ZAxisRotationTransform(0.0);
	eae6320::Collision::Matrix4x4 aTrans = eae6320::Collision::Matrix4x4::TranslationTransform(x,y, 0.0f);
	eae6320::Collision::Matrix4x4 aRots = eae6320::Collision::Matrix4x4::ZAxisRotationTransform(0.0);

	eae6320::Collision::Matrix4x4 bToWorld = bTrans * bRots;
	eae6320::Collision::Matrix4x4 worldToB = bToWorld.Inverse();

	eae6320::Collision::Matrix4x4 aToWorld = aTrans * aRots;
	eae6320::Collision::Matrix4x4 worldToA = aToWorld.Inverse();

	eae6320::Collision::Matrix4x4 aToB = worldToB * aToWorld;
	eae6320::Collision::Matrix4x4 bToA = worldToA * bToWorld;

	//A projects to B

	eae6320::Collision::Point4D temp;
	temp = eae6320::Collision::Point4D(x, y, 0.0f);
	eae6320::Collision::Point4D aOriginToB = aToB * temp;
	temp = Point4D((*rect).width, 0.0f, 0.0f, 0.0f);
	Point4D aWidthToB = aToB * temp;
	temp = Point4D(0.0f, (*rect).height, 0.0f, 0.0f);
	Point4D aHeightToB = aToB * temp;

	//X axis
	double aToBX = fabs((double) aWidthToB[0]) + fabs((double) aHeightToB[0]);
	double bExt = (*(*other).rect).width + aToBX;
	double bLeft = (*other).x - bExt; 
	double bRight = (*other).x + bExt;

	if (aOriginToB[0] < bLeft || aOriginToB[0] > bRight)
	{
		return false;
	}

		//Y axis
	double aToBY = fabs((double)aWidthToB[1]) + fabs((double)aHeightToB[1]);
	bExt = (*(*other).rect).height + aToBY;
	double bDown = (*other).y - bExt;
	double bUp = (*other).y + bExt;

	if (aOriginToB[1] < bDown || aOriginToB[1] > bUp)
	{
		return false;
	}

	//B projects to A
	temp = eae6320::Collision::Point4D((*other).x, (*other).y, 0.0f);
	eae6320::Collision::Point4D bOriginToA = bToA * temp;
	temp = Point4D((*(*other).rect).width, 0.0f, 0.0f, 0.0f);
	Point4D bWidthToA = bToA * temp;
	temp = Point4D(0.0f, (*(*other).rect).height, 0.0f, 0.0f);
	Point4D bHeightToA = bToA * temp;

	//X axis
	double bToAX = fabs((double)bWidthToA[0]) + fabs((double)bHeightToA[0]);
	double aExt = (*rect).width + bToAX;
	double aLeft = x - aExt;
	double aRight = x + aExt;

	if (bOriginToA[0] < aLeft || bOriginToA[0] > aRight)
		return false;

	//Y axis
	double bToAY = fabs((double)bWidthToA[1]) + fabs((double)bHeightToA[1]);
	aExt = (*rect).height + bToAY;
	double aDown = y - aExt;
	double aUp = y + aExt;

	if (bOriginToA[1] < aDown || bOriginToA[1] > aUp)
		return false;

	return true;
}

bool eae6320::Collision::Collider::Check2CircleCollision(eae6320::Collision::Collider* other)
{
	eae6320::Collision::Matrix4x4 bTrans = eae6320::Collision::Matrix4x4::TranslationTransform((*other).x, (*other).y, 0.0f);
	eae6320::Collision::Matrix4x4 bRots = eae6320::Collision::Matrix4x4::ZAxisRotationTransform(0.0);
	eae6320::Collision::Matrix4x4 aTrans = eae6320::Collision::Matrix4x4::TranslationTransform(x, y, 0.0f);
	eae6320::Collision::Matrix4x4 aRots = eae6320::Collision::Matrix4x4::ZAxisRotationTransform(0.0);

	eae6320::Collision::Matrix4x4 bToWorld = bTrans * bRots;
	eae6320::Collision::Matrix4x4 worldToB = bToWorld.Inverse();

	eae6320::Collision::Matrix4x4 aToWorld = aTrans * aRots;
	eae6320::Collision::Matrix4x4 worldToA = aToWorld.Inverse();

	eae6320::Collision::Matrix4x4 aToB = worldToB * aToWorld;
	eae6320::Collision::Matrix4x4 bToA = worldToA * bToWorld;

	//A projects to B

	eae6320::Collision::Point4D temp;
	temp = eae6320::Collision::Point4D(x, y, 0.0f);
	eae6320::Collision::Point4D aOriginToB = aToB * temp;

	if (pow(((*circle).radius - (*(*other).circle).radius), 2) <= (pow((aOriginToB[0] - (*other).x),2) + pow((aOriginToB[1] - (*other).y), 2)) && (pow((aOriginToB[0] - (*other).x), 2) + pow((aOriginToB[1] - (*other).y), 2))  <= pow(((*circle).radius + (*(*other).circle).radius), 2))
	{
		return true;
	}

	//B projects to A
	temp = eae6320::Collision::Point4D((*other).x, (*other).y, 0.0f);
	eae6320::Collision::Point4D bOriginToA = bToA * temp;

	if (pow(((*(*other).circle).radius - (*circle).radius), 2) <= (pow((bOriginToA[0] - x), 2) + pow((bOriginToA[1] - y), 2)) && (pow((bOriginToA[0] - x), 2) + pow((bOriginToA[1] - y), 2)) <= pow(((*(*other).circle).radius + (*circle).radius), 2))
	{
		return true;
	}

	return false;
}

bool eae6320::Collision::Collider::CheckCircleRectCollision(eae6320::Collision::Collider* other)
{
	eae6320::Collision::Matrix4x4 bTrans = eae6320::Collision::Matrix4x4::TranslationTransform((*other).x, (*other).y, 0.0f);
	eae6320::Collision::Matrix4x4 bRots = eae6320::Collision::Matrix4x4::ZAxisRotationTransform(0.0);
	eae6320::Collision::Matrix4x4 aTrans = eae6320::Collision::Matrix4x4::TranslationTransform(x, y, 0.0f);
	eae6320::Collision::Matrix4x4 aRots = eae6320::Collision::Matrix4x4::ZAxisRotationTransform(0.0);

	eae6320::Collision::Matrix4x4 bToWorld = bTrans * bRots;
	eae6320::Collision::Matrix4x4 worldToB = bToWorld.Inverse();

	eae6320::Collision::Matrix4x4 aToWorld = aTrans * aRots;
	eae6320::Collision::Matrix4x4 worldToA = aToWorld.Inverse();

	eae6320::Collision::Matrix4x4 aToB = worldToB * aToWorld;
	eae6320::Collision::Matrix4x4 bToA = worldToA * bToWorld;

	//A projects to B
	eae6320::Collision::Point4D temp;
	temp = eae6320::Collision::Point4D(x, y, 0.0f);
	eae6320::Collision::Point4D aOriginToB = aToB * temp;

	float distx = abs(aOriginToB[0] - (*other).x);
	float disty = abs(aOriginToB[1] - (*other).y);

	if (distx > ((*(*other).rect).width + (*circle).radius)) { return false; }
	if (disty > ((*(*other).rect).height + (*circle).radius)) { return false; }

	if (distx <= ((*(*other).rect).width)) { return true; }
	if (disty <= ((*(*other).rect).height)) { return true; }

	double cornerDistance_sq = pow((distx - (*(*other).rect).width),2) +
		pow((disty - (*(*other).rect).height),2);

	return (cornerDistance_sq <= pow((*circle).radius,2));
}

eae6320::cResult eae6320::Collision::Collider::Load(const std::string& i_path, eae6320::Collision::Collider*& o_Collider)
{
	eae6320::Platform::sDataFromFile dataFromFile;
	eae6320::Platform::LoadBinaryFile(i_path.c_str(), dataFromFile);

	bool circle;
	float width = 0.0f;
	float height = 0.0f;
	float radius = 0.0f;
	uint16_t layer = 0;
	bool check = false;

	if (dataFromFile.data)
	{
		auto currentOffset = reinterpret_cast<uintptr_t>(dataFromFile.data);
		circle = *reinterpret_cast<bool*>(currentOffset);
		currentOffset += sizeof(circle);

		if (circle)
		{
			radius = *reinterpret_cast<float*>(currentOffset);
			currentOffset += sizeof(radius);
		}
		else
		{
			width = *reinterpret_cast<float*>(currentOffset);
			currentOffset += sizeof(width);

			height = *reinterpret_cast<float*>(currentOffset);
			currentOffset += sizeof(height);
		}

		layer = *reinterpret_cast<uint16_t*>(currentOffset);
		check = true;
	}

	if (!check)
	{
		return eae6320::Results::Failure;
	}

	if (circle)
	{
		return eae6320::Collision::Collider::FactoryHelper(o_Collider, radius, layer, currId++);
	}
	else
	{
		return eae6320::Collision::Collider::FactoryHelper(o_Collider, width , height, layer, currId++);
	}
}

//updates the position of the collider
void eae6320::Collision::Collider::UpdatePosition(float i_x, float i_y)
{
	x = i_x;
	y = i_y;
}

uint16_t eae6320::Collision::Collider::GetID()
{
	return id;
}

uint16_t eae6320::Collision::Collider::GetLayer()
{
	return layer;
}

void eae6320::Collision::Collider::SetLayer(uint16_t i_layer)
{
	layer = i_layer;
}


std::vector<eae6320::CollisionSystem::CollisionPair> eae6320::CollisionSystem::Run(std::vector<eae6320::Collision::Collider*> colliderList)
{
	std::vector<eae6320::CollisionSystem::CollisionPair> temp;

	for (size_t a = 0; a < colliderList.size() - 1; a++)
	{
		for (size_t b = a + 1; b < colliderList.size(); b++)
		{
			if ((*colliderList[a]).GetLayer() == (*colliderList[b]).GetLayer() && (*colliderList[a]).CheckCollisions(colliderList[b]))
			{
				eae6320::CollisionSystem::CollisionPair tempPair;
				(tempPair).idA = colliderList[a]->GetID();
				(tempPair).idB = colliderList[b]->GetID();
				temp.push_back(tempPair);
			}
		}
	}
	return temp;
}
