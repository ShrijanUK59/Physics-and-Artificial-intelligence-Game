#pragma once
#include "GameObject.h"
namespace NCL {
	namespace CSC8503 {
		class TileGround : public GameObject {
		public:
			TileGround() {
				invMass = 0.0f;		
				elasticity = 0.0f;
				friction = 0.1f;
				name = "Tile Ground";
			}
			void SetPhysicsObject(PhysicsObject* newObject) override {
				physicsObject = newObject;
				physicsObject->SetInverseMass(invMass);
				physicsObject->SetElasticity(elasticity); 
				physicsObject->SetFriction(friction);
			}
		protected:
			float invMass;
			float elasticity;
			float friction;
		};

	}
}