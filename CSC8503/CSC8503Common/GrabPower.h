#pragma once
#include "GameObject.h"
#include "PlayerBall.h"
#include "GameObjectEnemy.h"
namespace NCL {
	namespace CSC8503 {
		class GrabPower : public GameObject {
		public:
			GrabPower() {
				name = "Pickup";
				invMass = 0.0f;		
				elasticity = 0.0;
				friction = 0.8;
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

