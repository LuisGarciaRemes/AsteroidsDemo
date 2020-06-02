#pragma once
#include <Engine\Physics\sRigidBodyState.h>
#include <Engine\Results\Results.h>
#include <Engine\Math\cMatrix_transformation.h>
namespace eae6320
{
	namespace Graphics
	{
		class cCamera
		{
			public:
				static eae6320::cResult CreateCamera(cCamera*& o_cCamera)
				{
					o_cCamera = new cCamera();

					auto result = eae6320::Results::Success;

					if (o_cCamera == nullptr)
					{
						result = eae6320::Results::Failure;
					}

					return result;
				}

				void Update(const float i_elapsedSecondCount_sinceLastUpdate);
				eae6320::Physics::sRigidBodyState* GetRigidBody();
				eae6320::Math::cMatrix_transformation GetWorldToCamera();
				eae6320::Math::cMatrix_transformation GetCameraToProjected();

				void SetVelocity(eae6320::Math::sVector vel);
				
			private:
				cCamera();
				~cCamera();
				eae6320::Physics::sRigidBodyState* m_rigidbody;
				eae6320::Math::cMatrix_transformation m_worldToCamera;
				eae6320::Math::cMatrix_transformation m_cameraToProjected;
				float m_verticalFieldOfView_inRadians;
				float m_aspectRatio;
				float m_z_nearPlane;
				float m_z_farPlane;
				float DegreesToRadians(float degrees);
		};
	}
}

