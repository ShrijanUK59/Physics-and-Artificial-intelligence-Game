#pragma once
#include "../CSC8503Common/GameWorld.h"
#include <set>
#include "ThisSpring.h"
#include "PhysicsObject.h"
#include "GameObject.h"
#include "CollisionDetection.h"
#include "../../Common/Quaternion.h"
#include "Constraint.h"
#include "Debug.h"
#include <functional>
#include "ThisSpring.h"
#include "SpinningTile.h"
#include "GroundObject.h"
#include "GrabPower.h"
namespace NCL {
	namespace CSC8503 {
		class PhysicsSystem {
		public:
			PhysicsSystem(GameWorld& g);
			~PhysicsSystem();

			void Clear();
			void ClearDeletedCollisions();
			void Update(float dt);

			void UseGravity(bool state) {
				applyGravity = state;
			}

			void UseBroadphase(bool state) {
				useBroadPhase = state;
			}

			void SetGlobalDamping(float d) {
				globalDamping = d;
			}

			void SetGravity(const Vector3& g) {
				gravity = g;
			}

			float GetDampingFactor() const {
				return dampingFactor;
			}

			void SetDampingFactor(float val) {
				dampingFactor = val;
			}

			bool GetBroadPhase() const {
				return useBroadPhase;
			}

			void SetBroadPhase(bool val) {
				useBroadPhase = val;
			}

			void DecreaseIterationCount() {
				constraintIterationCount--;
			}

			void IncreaseIterationCount() {
				constraintIterationCount++;
			}

			int GetConstraintIterationCount() const {
				return constraintIterationCount;
			}

			int GetBasicCollisionsTested() const {
				return basicCollisionsTested;
			}

			int GetBroadPhaseCollisionsTested() const {
				return broadphaseCollisions.size();
			}

			int GetCollisionsSize() const {
				return totalCollisions;
			}

		protected:
			void BasicCollisionDetection();
			void BroadPhase();
			void NarrowPhase();

			void ClearForces();

			void IntegrateAccel(float dt);
			void IntegrateVelocity(float dt);

			void UpdateConstraints(float dt);

			void UpdateCollisionList();
			void UpdateObjectAABBs();

			void ImpulseResolveCollision(GameObject& a, GameObject& b, CollisionDetection::ContactPoint& p) const;
			void SpringTowardsPoint(ThisSpring* a) const;

			GameWorld& gameWorld;

			bool	applyGravity;
			Vector3 gravity;
			float	dTOffset;
			float	globalDamping;
			float	dampingFactor;
			std::set<CollisionDetection::CollisionInfo> allCollisions;
			std::set<CollisionDetection::CollisionInfo> broadphaseCollisions;
			vector<GameObject*> staticObjects;
			vector<GameObject*> dynamicObjects;
			bool useBroadPhase;
			int numCollisionFrames = 5;
			int constraintIterationCount = 10;

			int basicCollisionsTested;
			int totalCollisions;
		};
	} 
}

