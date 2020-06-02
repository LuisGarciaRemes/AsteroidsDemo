#pragma once
#include "Engine/Graphics/cEffect.h"
#include "Engine/Graphics/cGeometry.h"
#include "Engine/Collision/Collider.h"
#include <Engine\Physics\sRigidBodyState.h>
#include <Engine\Graphics\ConstantBufferFormats.h>
namespace eae6320
{
	namespace MyGame
	{
		class cGameObject
		{
		public:
			static eae6320::cResult CreateGameObject(cGameObject*& o_cGameObject)
			{
				o_cGameObject = new cGameObject();

				auto result = eae6320::Results::Success;

				if (o_cGameObject == nullptr)
				{
					result = eae6320::Results::Failure;
				}

				return result;
			}

			void SetGeometryAndEffect(eae6320::Graphics::cGeometry* i_geometry, eae6320::Graphics::cEffect* i_effect);
			void SetGeometryAndEffectAndCollider(eae6320::Graphics::cGeometry* i_geometry, eae6320::Graphics::cEffect* i_effect, eae6320::Collision::Collider* collider);

			bool IsEnabled();
			void SetEnabled(bool i_render);

			eae6320::Graphics::cGeometry* GetGeometry();

			eae6320::Graphics::cEffect* GetEffect();

			eae6320::Physics::sRigidBodyState* GetRigidBody();

			eae6320::Collision::Collider* GetCollider();

			void SetAcceleration(eae6320::Math::sVector acc);

			void SetVelocity(eae6320::Math::sVector vel);

			void Update(const float i_elapsedSecondCount_sinceLastUpdate);

			eae6320::Math::cMatrix_transformation GetMatrix();

			std::string tag;

			void ResetRigidBody();

			void SetCollider(eae6320::Collision::Collider* i_collider);

		private:
			cGameObject();
			~cGameObject();
			eae6320::Graphics::cGeometry* m_geometry;
			eae6320::Graphics::cEffect* m_effect;
			eae6320::Physics::sRigidBodyState* m_rigidbody;
			eae6320::Collision::Collider* m_collider;
			eae6320::Math::cMatrix_transformation matrix;
			bool enabled;
		};
	}
}

