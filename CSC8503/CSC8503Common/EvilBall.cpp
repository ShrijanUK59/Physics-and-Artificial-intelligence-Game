#include "EvilBall.h"
#include <algorithm>
using namespace NCL;
using namespace CSC8503;
EvilBall::EvilBall(vector<Vector3> positions) : GameObjectEnemy() {
	currentState = state::FOLLOWROUTE;
	route = positions;
	currentDest = 0;
	routeTimeout = 0.0f;
	backwards = false;
	patrolState = new State([&](float dt)->void {
		this->Patrol(dt);
		if (displayPath)
			this->DisplayRoute();
		});
	stateMachine->AddState(patrolState);
	stateMachine->AddTransition(new StateTransition(followObjectState, patrolState, [&]()->bool {
		if (this->followTimeout < 0.0f) {
			currentState = state::FOLLOWROUTE;
			return true;
		}
		return false;
		}));
	stateMachine->AddTransition(new StateTransition(patrolState, followObjectState, [&]()->bool {
		if (this->followTimeout > 0.0f) {
			currentState = state::FOLLOWOBJECT;
			return true;
		}
		return false;
		}));
	stateMachine->AddTransition(new StateTransition(idleState, patrolState, [&]()->bool {
		if (this->route.size() > 0) {
			currentState = state::FOLLOWROUTE;
			return true;
		}
		return false;
		}));
	stateMachine->AddTransition(new StateTransition(patrolState, idleState, [&]()->bool {
		if (this->route.size() == 0) {
			currentState = state::IDLE;
			return true;
		}
		return false;
		}));
	name = "Evil Enemy Ball";
}

void EvilBall::Patrol(float dt) {
	routeTimeout += dt;
	if (routeTimeout > 10.0f) {
		GetTransform().SetPosition(route[currentDest] + Vector3(0, 10, 0));		
		routeTimeout = 0.0f;
	}
	travelDir = route[currentDest] - GetTransform().GetPosition();
	travelDir.y = 0;
	if (travelDir.Length() < 10.0f) {
		GetPhysicsObject()->ClearForces();
		routeTimeout = 0.0f;
		if (!backwards) {
			if (currentDest == route.size() - 1) {
				currentDest = route.size() - 2;
				backwards = !backwards;
			}
			else 
				currentDest++;
		}
		else {
			if (currentDest == 0) {		
				currentDest = 1;
				backwards = !backwards;
			}
			else 
				currentDest--;
		}
	}
}

void EvilBall::DisplayRoute() {
	for (int i = 0; i < route.size() - 1; ++i) {
		if (!backwards) {
			if (i + 1 == currentDest)
				Debug::DrawLine(route[i], route[i + 1], Debug::CYAN);
			else
				Debug::DrawLine(route[i], route[i + 1], Debug::WHITE);
		}
		else {
			if (i == currentDest)
				Debug::DrawLine(route[i], route[i + 1], Debug::CYAN);
			else
				Debug::DrawLine(route[i], route[i + 1], Debug::WHITE);
		}
	}
} 