#pragma once
#include "GameObject.h"
namespace NCL {
	namespace CSC8503 {
		class ThisSpring : public GameObject {
		public:
			ThisSpring(Vector3 rest, float springK) {
				invMass = 25.0f;		
				elasticity = 10.0f;
				friction = 0.0;
				k = springK;
				name = "Solgaleo Object";
				restPosition = rest;
			}
			void SetPhysicsObject(PhysicsObject* newObject) override {
				physicsObject = newObject;
				physicsObject->SetInverseMass(invMass);
				physicsObject->SetElasticity(elasticity);
				physicsObject->SetFriction(friction);
			}
			Vector3 GetRestPosition() const {
				return restPosition;
			}
			void SetRestPosition(Vector3 val) {
				restPosition = val;
			}
			float GetK() const {
				return k;
			}
		protected:
			float invMass;
			float elasticity;
			float friction;
			float k;
			Vector3 restPosition; 
		};
	}
}