#pragma once
#include "TileGround.h"
#include "PlayerBall.h"
#include "EvilBallEnemy.h"
#include "EvilBall.h"
#include "EnemyBallBehaviourTree.h"

namespace NCL {
	namespace CSC8503 {
		class GroundObject : public TileGround {
		public:
			GroundObject() {
				name = "Portal";
			}
			void OnCollisionBegin(GameObject* otherObject) override {
				otherObject->GetPhysicsObject()->ClearForces();
				otherObject->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
				otherObject->GetPhysicsObject()->SetAngularVelocity(Vector3(0, 0, 0));
				if (dynamic_cast<PlayerBall*>(otherObject)) 
					((PlayerBall*)otherObject)->GetTransform().SetPosition(((PlayerBall*)otherObject)->GetSpawnPos());		
				else if (dynamic_cast<GameObjectEnemy*>(otherObject)) {
					((GameObjectEnemy*)otherObject)->ClearFollowObjects();
					if (dynamic_cast<EvilBallEnemy*>(otherObject))
						((EvilBallEnemy*)otherObject)->GetTransform().
						SetPosition(((EvilBallEnemy*)otherObject)->GetPath()[0] + Vector3(0, 10, 0));
					if (dynamic_cast<EvilBall*>(otherObject))
						((EvilBall*)otherObject)->GetTransform().SetPosition(((EvilBall*)otherObject)
							->GetRoute()[((EvilBall*)otherObject)->GetCurrentDest()] + Vector3(0, 10, 0));
				}
				else if (dynamic_cast<EnemyBallBehaviourTree*>(otherObject)) {
					((EnemyBallBehaviourTree*)otherObject)->GetTransform().SetPosition(((EnemyBallBehaviourTree*)otherObject)->GetStartPosition());
					((EnemyBallBehaviourTree*)otherObject)->SetReset(true);
				}
				else if (!dynamic_cast<ObjectStateGame*>(otherObject))		
					otherObject->SetIsActive(false); 
			}
		};
	}
}