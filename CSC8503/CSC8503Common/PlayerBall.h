#pragma once
#include "GameObject.h"
namespace NCL {
	namespace CSC8503 {
		class PlayerBall : public GameObject {
		public:
			PlayerBall() {
				name = "Player Ball";
				invMass = 7.0f;
				elasticity = 0.3;
				friction = 0.4;
				canJump = false;
				score = 300;
				finished = false;
			}
			void SetPhysicsObject(PhysicsObject* newObject) override {
				physicsObject = newObject;
				physicsObject->SetInverseMass(invMass);
				physicsObject->SetElasticity(elasticity);
				physicsObject->SetFriction(friction);
			}
			void OnCollisionBegin(GameObject* otherObject) override {
				canJump = true;
			}
			void OnCollisionEnd(GameObject* otherObject) override {
				
			}
			void Jump() {
				if (canJump) {
					physicsObject->ApplyLinearImpulse(Vector3(0, 9, 0));
					canJump = false;
				}
			}
			int GetScore() const {
				return score;
			}
			void BonusAcquired() {
				score += 15;
			}
			
			void DecreaseScore(float dt) {
				score -= (1 * dt);
			}
			bool GetFinished() const {
				return finished;
			}
			void SetFinished(bool val) {
				finished = val;
			}
			Vector3 GetSpawnPos() const {
				return spawnPos;
			}
			void SetSpawnPos(Vector3 val) {
				spawnPos = val;
			}
			void Update(float dt) {
				if (powerUpTimer > 0.0f) 
					powerUpTimer -= dt;
				else
					GetRenderObject()->SetColour(Vector4(2.2, 3.4, 1.9, 4));
			}
		protected:
			float invMass;
			float elasticity;
			float friction;
			bool canJump;
			float score;
			bool finished;
			Vector3 spawnPos; 
		};
	}
}