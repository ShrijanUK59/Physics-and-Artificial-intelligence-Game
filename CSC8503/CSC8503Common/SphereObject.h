#pragma once
#include "GameObject.h"
namespace NCL {
	namespace CSC8503 {
		class SphereObject : public GameObject {
		public:
			SphereObject() {
				invMass = 5.0f;
				elasticity = 0.2;
				friction = 0.2;
				name = "Sphere";
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