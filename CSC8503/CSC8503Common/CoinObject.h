#pragma once
#include "GrabPower.h"
namespace NCL {
	namespace CSC8503 {
		class CoinObject : public GrabPower {
		public: 
			CoinObject() {
				name = "Power Up";
			}
			
			void OnCollisionBegin(GameObject* otherObject) override {
				if (dynamic_cast<PlayerBall*>(otherObject) || dynamic_cast<GameObjectEnemy*>(otherObject) ||
					dynamic_cast<EnemyBallBehaviourTree*>(otherObject))
					isActive = false;
				if (dynamic_cast<PlayerBall*>(otherObject))
					((PlayerBall*)otherObject)->BonusAcquired();		
			} 
		};
	}
}