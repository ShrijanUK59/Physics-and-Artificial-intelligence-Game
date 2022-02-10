#pragma once
#include "PlayerBall.h"
#include "GrabPower.h"
namespace NCL {
	namespace CSC8503 {
		class PowerupObject : public GrabPower {
		public:
			PowerupObject() {
				name = "Power Up";
			}
			
			void OnCollisionBegin(GameObject* otherObject) override {
				if (dynamic_cast<PlayerBall*>(otherObject) || dynamic_cast<GameObjectEnemy*>(otherObject) ||
					dynamic_cast<EnemyBallBehaviourTree*>(otherObject))
					isActive = false;
				if (dynamic_cast<GameObjectEnemy*>(otherObject) || dynamic_cast<EnemyBallBehaviourTree*>(otherObject)) {
					otherObject->SetPowerUpTimer(10.0f);
					otherObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				}
				else if (dynamic_cast<PlayerBall*>(otherObject)) {
					otherObject->SetPowerUpTimer(10.0f);
					otherObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
					if (dynamic_cast<PlayerBall*>(otherObject))
						((PlayerBall*)otherObject)->BonusAcquired();
				}
			}
		};
	}
}