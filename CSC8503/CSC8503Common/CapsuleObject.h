#pragma once
#include "GameObject.h"
namespace NCL {
	namespace CSC8503 {
		class CapsuleObject : public GameObject {
		public:
			CapsuleObject() {
				invMass = 5.0f;
				elasticity = 0.2;
				friction = 0.2;
				name = "Capsule";
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