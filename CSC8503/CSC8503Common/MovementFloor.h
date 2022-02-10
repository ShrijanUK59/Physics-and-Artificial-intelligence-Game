#pragma once
#include "ObjectStateGame.h"
namespace NCL {
	namespace CSC8503 {
		class MovementFloor : public ObjectStateGame {
		public:
			MovementFloor(Vector3 start, Vector3 end);
			void SetPhysicsObject(PhysicsObject* newObject) override {
				physicsObject = newObject;
				physicsObject->SetInverseMass(0.0f);
				physicsObject->SetElasticity(10.0f);
				physicsObject->SetFriction(0.8f);
			}
			void MoveToLocation(float dt);
		protected:
			State* followRouteState;
			Vector3 speed;
			Vector3 start;
			Vector3 end; 
			float invMass;
			float elasticity;
			float friction;
		};
	}
}
