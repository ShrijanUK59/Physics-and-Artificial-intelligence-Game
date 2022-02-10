#pragma once
#include "TileGround.h"
#include "EvilBallEnemy.h"
namespace NCL {
	namespace CSC8503 {
		class GameEnd : public TileGround {
		public:
			GameEnd() {
				name = "End"; 
			}
			
			void OnCollisionBegin(GameObject* otherObject) override {
				if (dynamic_cast<PlayerBall*>(otherObject))
					((PlayerBall*)otherObject)->SetFinished(true);
				else if (dynamic_cast<EvilBallEnemy*>(otherObject))
					((EvilBallEnemy*)otherObject)->SetFinished(true);
			}
		};
	}
}