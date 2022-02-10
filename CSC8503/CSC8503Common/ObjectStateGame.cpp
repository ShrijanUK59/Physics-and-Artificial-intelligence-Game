#include "ObjectStateGame.h"
using namespace NCL;
using namespace CSC8503;
ObjectStateGame::ObjectStateGame() {
	stateMachine = new StateMachine();
	currentState = state::IDLE;
	idleState = new State([&](float dt)->void {
		this->Idle();
	});
	stateMachine->AddState(idleState);
}

ObjectStateGame::~ObjectStateGame() {
	delete stateMachine;
}

void ObjectStateGame::Update(float dt) {
	stateMachine->Update(dt);
}

string ObjectStateGame::StateToString() const {
	switch (currentState) {
		case state::IDLE:
			return "Searching";
		case state::FOLLOWROUTE:
			return "Pathfinding";
		case state::FOLLOWPATH:
			return "Pathfinding Route";
		case state::FOLLOWOBJECT:
			return "Enemy Ball Spotted Player"; 
		default:
			return "Unknown Status";
	}
}

void ObjectStateGame::Idle() {
	GetPhysicsObject()->ClearForces();
}