#pragma once
#include "GameObject.h"
namespace NCL {
	namespace CSC8503 {
		class EnemyObject : public GameObject {
		public:
			EnemyObject() {
				name = "Evil Enemy Ball";
				invMass = 7.0f; 
				elasticity = 0.2;
				friction = 0.2; 
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