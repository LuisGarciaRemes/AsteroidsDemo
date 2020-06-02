#include "cCamera.h"

eae6320::Physics::sRigidBodyState* eae6320::Graphics::cCamera::GetRigidBody()
{
	return m_rigidbody;
}

void eae6320::Graphics::cCamera::Update(const float i_elapsedSecondCount_sinceLastUpdate)
{
	m_rigidbody->PredictFuturePosition(i_elapsedSecondCount_sinceLastUpdate);
	m_rigidbody->Update(i_elapsedSecondCount_sinceLastUpdate);
	m_cameraToProjected = eae6320::Math::cMatrix_transformation::CreateCameraToProjectedTransform_perspective(m_verticalFieldOfView_inRadians,m_aspectRatio,m_z_nearPlane,m_z_farPlane);
	m_worldToCamera = eae6320::Math::cMatrix_transformation::CreateWorldToCameraTransform(m_rigidbody->orientation,m_rigidbody->position);
}

eae6320::Math::cMatrix_transformation eae6320::Graphics::cCamera::GetWorldToCamera()
{
	return m_worldToCamera;
}

eae6320::Math::cMatrix_transformation eae6320::Graphics::cCamera::GetCameraToProjected()
{
	return m_cameraToProjected;
}

void eae6320::Graphics::cCamera::SetVelocity(eae6320::Math::sVector vel)
{
	m_rigidbody->velocity = vel;
}

eae6320::Graphics::cCamera::cCamera()
{
	m_rigidbody = new eae6320::Physics::sRigidBodyState();
	m_rigidbody->position = eae6320::Math::sVector(0,0,10);
	m_rigidbody->orientation = eae6320::Math::cQuaternion();
	m_verticalFieldOfView_inRadians = DegreesToRadians(45);
	m_z_nearPlane = 0.1f;
	m_z_farPlane = 100.0f;
	m_aspectRatio = 1.0f;
}

eae6320::Graphics::cCamera::~cCamera()
{
}

float eae6320::Graphics::cCamera::DegreesToRadians(float degrees)
{
	return degrees * 0.0174533f;
}
