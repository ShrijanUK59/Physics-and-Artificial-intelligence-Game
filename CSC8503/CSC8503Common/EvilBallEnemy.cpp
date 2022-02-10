#include "EvilBallEnemy.h"
#include <algorithm>
#include "../../Common/Maths.h"
using namespace NCL;
using namespace CSC8503;
EvilBallEnemy::EvilBallEnemy(bool ignore) : GameObjectEnemy() {
	currentState = state::FOLLOWPATH;
	ignoreCosts = ignore;
	mazeStart = { 220, 0, 440 }; 
	mazeEnd = { 220, 0, 220 };
	pathTimeout = 0.0f;
	FindPath();
	followPathState = new State([&](float dt)->void {
		this->FollowPath(dt);
		if (displayPath)
			this->DisplayPath();
		});
	stateMachine->AddState(followPathState);
	stateMachine->AddTransition(new StateTransition(idleState, followPathState, [&]()->bool {
		if (this->path.size() > 0) {
			currentState = state::FOLLOWPATH;
			return true;
		}
		return false;
		}));
	stateMachine->AddTransition(new StateTransition(followPathState, idleState, [&]()->bool {
		if (this->path.size() == 0) {
			currentState = state::IDLE;
			return true;
		}
		return false;
		}));
	stateMachine->AddTransition(new StateTransition(followObjectState, followPathState, [&]()->bool {
		if (this->followTimeout < 0.0f) {
			interestObjects.erase(currentObject);
			currentState = state::FOLLOWPATH;
			return true;
		}
		return false;
		}));
	stateMachine->AddTransition(new StateTransition(followPathState, followObjectState, [&]()->bool {
		if (this->followTimeout > 0.0f) {
			currentState = state::FOLLOWOBJECT;
			return true;
		}
		return false;
		}));
	name = "Evil Enemy Ball";
}

void EvilBallEnemy::FindPath() {
	NavigationPath outPath;
	NavigationGrid grid("MazePath.txt");
	bool found = grid.FindPath(mazeStart, mazeEnd, outPath, ignoreCosts);
	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		path.push_back(pos - (mazeStart + Vector3(0, 0, 20)));		
	}
}

void EvilBallEnemy::DisplayPath() {
	for (int i = 1; i < path.size(); ++i) {
		Debug::DrawLine(path[i - 1] + Vector3(0, 10, 0), path[i] + Vector3(0, 10, 0), Debug::WHITE);
	}
}

void EvilBallEnemy::FollowPath(float dt) {
	pathTimeout += dt;
	if (pathTimeout > 10.0f) {
		GetTransform().SetPosition(path[0] + Vector3(0, 10, 0));		 
		pathTimeout = 0.0f;
	}
	if (path.size() > 0) {
		travelDir = path[0] - GetTransform().GetPosition();
		travelDir.y = 0;
		if (travelDir.Length() < 10.0f) {
			GetPhysicsObject()->ClearForces();
			pathTimeout = 0.0f;
			path.erase(path.begin());
			if (path.size() == 0)		
				return;
		}
	}
}
