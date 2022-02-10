#pragma once
#include "GameObject.h"
namespace NCL {
	namespace CSC8503 {
		class CubeObject : public GameObject {
		public:
			CubeObject() {
				invMass = 2.0f;
				elasticity = 10.0f;
				friction = 0.2; 
				name = "Object Cube";
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