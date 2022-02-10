#pragma once
#include "GameObjectEnemy.h"
#include "Debug.h"
#include "NavigationGrid.h"
#include "PlayerBall.h"
namespace NCL {
	namespace CSC8503 {
		class EvilBallEnemy : public GameObjectEnemy { 
		public:
			EvilBallEnemy(bool ignoreCosts);
			vector<Vector3> GetPath() const {
				return path;
			}
			void OnCollisionBegin(GameObject* otherObject) override {
				otherObject->GetPhysicsObject()->ClearForces();
				otherObject->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
				otherObject->GetPhysicsObject()->SetAngularVelocity(Vector3(0, 0, 0));
				if (dynamic_cast<PlayerBall*>(otherObject))
					((PlayerBall*)otherObject)->GetTransform().SetPosition(((PlayerBall*)otherObject)->GetSpawnPos());		
			}
		protected:
			void FindPath();
			void DisplayPath();
			void FollowPath(float dt);
			vector<Vector3> path;
			Vector3 mazeStart;
			Vector3 mazeEnd;
			State* followPathState;
			bool ignoreCosts;
			float pathTimeout;
		};
	}
}
