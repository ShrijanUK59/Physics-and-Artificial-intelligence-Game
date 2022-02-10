#pragma once
#include "GameObject.h"
#include "StateMachine.h"
namespace NCL {
	namespace CSC8503 {
		enum class state { IDLE, FOLLOWROUTE, FOLLOWPATH, FOLLOWOBJECT };
		class StateMachine;
		class ObjectStateGame : public GameObject {
		public:
			ObjectStateGame();
			~ObjectStateGame();
			virtual void Update(float dt);
			string StateToString() const;
			void Idle();
		protected:
			state currentState; 
			StateMachine* stateMachine;
			State* idleState;
		};
	}
}