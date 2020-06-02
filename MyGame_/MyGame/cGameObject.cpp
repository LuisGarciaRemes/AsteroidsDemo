#include "cGameObject.h"

bool eae6320::MyGame::cGameObject::IsEnabled()
{
	return enabled;
}

void eae6320::MyGame::cGameObject::SetEnabled(bool i_render)
{
	enabled = i_render;
}

eae6320::Graphics::cGeometry* eae6320::MyGame::cGameObject::GetGeometry()
{
	return m_geometry;
}

eae6320::Graphics::cEffect* eae6320::MyGame::cGameObject::GetEffect()
{
	return m_effect;
}

eae6320::Physics::sRigidBodyState* eae6320::MyGame::cGameObject::GetRigidBody()
{
	return m_rigidbody;
}

eae6320::Collision::Collider* eae6320::MyGame::cGameObject::GetCollider()
{
	return m_collider;
}

void eae6320::MyGame::cGameObject::SetAcceleration(eae6320::Math::sVector acc)
{
	m_rigidbody->acceleration = acc;
}

void eae6320::MyGame::cGameObject::SetVelocity(eae6320::Math::sVector vel)
{
	m_rigidbody->velocity = vel;
}


void eae6320::MyGame::cGameObject::Update(const float i_elapsedSecondCount_sinceLastUpdate)
{
	matrix = m_rigidbody->PredictFutureTransform(i_elapsedSecondCount_sinceLastUpdate);
	if (m_collider)
	{
		m_collider->UpdatePosition(m_rigidbody->position.x, m_rigidbody->position.y);
	}
	m_rigidbody->Update(i_elapsedSecondCount_sinceLastUpdate);
}

eae6320::Math::cMatrix_transformation eae6320::MyGame::cGameObject::GetMatrix()
{
	return matrix;
}

void eae6320::MyGame::cGameObject::ResetRigidBody()
{
	m_rigidbody = new eae6320::Physics::sRigidBodyState();
}

void eae6320::MyGame::cGameObject::SetCollider(eae6320::Collision::Collider* i_collider)
{
	m_collider = i_collider;
}

eae6320::MyGame::cGameObject::cGameObject()
{
	m_geometry = nullptr;
	m_effect = nullptr;
	m_rigidbody = new eae6320::Physics::sRigidBodyState();
	enabled = true;
	m_collider = nullptr;
}

eae6320::MyGame::cGameObject::~cGameObject()
{
}

void eae6320::MyGame::cGameObject::SetGeometryAndEffect(eae6320::Graphics::cGeometry* i_geometry, eae6320::Graphics::cEffect* i_effect)
{
	m_geometry = i_geometry;
	m_effect = i_effect;
	m_collider = nullptr;
}

void eae6320::MyGame::cGameObject::SetGeometryAndEffectAndCollider(eae6320::Graphics::cGeometry* i_geometry, eae6320::Graphics::cEffect* i_effect, eae6320::Collision::Collider* i_collider)
{
	m_geometry = i_geometry;
	m_effect = i_effect;
	m_collider = i_collider;
}
