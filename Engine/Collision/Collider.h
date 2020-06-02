#pragma once
#include <stdint.h>
#include <vector>
#include <string>
#include "../Results/Results.h"

namespace eae6320
{
	namespace Collision
	{
		class Collider
		{
		public:
			static eae6320::cResult CreateCollider(const std::string& i_path, eae6320::Collision::Collider*& o_Collider)
			{
				return Load(i_path, o_Collider);
			}
			Collider();
			~Collider();
			//Checks collision between to colliders
			bool CheckCollisions(eae6320::Collision::Collider* other);
			//Updates the position of the colllider
			void UpdatePosition(float x, float y);
			//Returns the id of the collider
			uint16_t GetID();
			//returns the layer of the collider
			uint16_t GetLayer();
			//sets the layer of the collider
			void SetLayer(uint16_t i_layer);


			//box collider
			struct BoxCollider
			{
				float width = 0.0f;
				float height = 0.0f;
			};

			//box collider
			struct CircleCollider
			{
				float radius = 0.0f;
			};


		private:
			static eae6320::cResult FactoryHelper(eae6320::Collision::Collider*& o_Collider, float i_radius, uint16_t i_layer, uint16_t i_id);
			static eae6320::cResult FactoryHelper(eae6320::Collision::Collider*& o_Collider, float i_width, float i_height, uint16_t i_layer, uint16_t i_id);
			eae6320::cResult InitializeCollider(float i_radius, uint16_t i_layer, uint16_t i_id);
			eae6320::cResult InitializeCollider(float i_width, float i_height, uint16_t i_layer, uint16_t i_id);
			bool Check2RectCollision(eae6320::Collision::Collider* other);
			bool Check2CircleCollision(eae6320::Collision::Collider* other);
			bool CheckCircleRectCollision(eae6320::Collision::Collider* other);
			static eae6320::cResult Load(const std::string& i_path, eae6320::Collision::Collider*& o_Collider);
			inline float GetWidth() { return (*rect).width; }
			inline float GetHeight() { return (*rect).height; }
			inline float GetRadius() { return (*circle).radius; }
			BoxCollider* rect;
			CircleCollider* circle;
			uint16_t layer;
			uint16_t id;
			float x;
			float y;
		};
	}

	namespace CollisionSystem
	{
		//box collider
		struct CollisionPair
		{
			uint16_t idA;
			uint16_t idB;
		};

		std::vector<eae6320::CollisionSystem::CollisionPair> Run(std::vector<eae6320::Collision::Collider*> colliderList);
	}
}

