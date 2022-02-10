#include "GameWorld.h"
#include "GameObject.h"
#include "Constraint.h"
#include "CollisionDetection.h"
#include "../../Common/Camera.h"
#include <algorithm> 
#include <random>
#include "PlayerBall.h"

using namespace NCL;
using namespace NCL::CSC8503;

GameWorld::GameWorld()	{
	mainCamera = new Camera();

	shuffleConstraints	= false;
	shuffleObjects		= false;
	worldIDCounter		= 0;
}

GameWorld::~GameWorld()	{
}

void GameWorld::Clear() {
	gameObjects.clear();
	constraints.clear();
}

void GameWorld::ClearAndErase() {
	for (auto& i : gameObjects) {
		delete i;
	}
	for (auto& i : constraints) {
		delete i;
	}
	Clear();
}

void GameWorld::AddGameObject(GameObject* o) {
	gameObjects.emplace_back(o);
	o->SetWorldID(worldIDCounter++);
}

void GameWorld::RemoveGameObject(GameObject* o, bool andDelete) {
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), o), gameObjects.end());
	if (andDelete)
		delete o;
}

void GameWorld::GetObjectIterators(GameObjectIterator& first, GameObjectIterator& last) const {
	first	= gameObjects.begin();
	last	= gameObjects.end();
}

void GameWorld::OperateOnContents(GameObjectFunc f) {
	for (GameObject* g : gameObjects) {
		f(g);
	}
}

void GameWorld::UpdateWorld(float dt) {
	std::random_device rd;
	std::mt19937 g(rd());
	if (shuffleObjects) 
		std::shuffle(gameObjects.begin(), gameObjects.end(), g);
	if (shuffleConstraints) 
		std::shuffle(constraints.begin(), constraints.end(), g);
	for (auto& i : gameObjects) {
		if (i->GetTimeAlive() > 40.0f)		
			i->SetIsActive(false);
		if (i->GetIsSafeForDeletion())		
			RemoveGameObject(i, true);
	}
}

void GameWorld::ShowFacing() {
	for (auto& i : gameObjects) {
		if(i->IsActive())
			Debug::DrawAxisLines(i->GetTransform().GetMatrix(), 2.0f);		
	}
}

bool GameWorld::Raycast(Ray& r, RayCollision& closestCollision, GameObject* current, bool closestObject) const {
	
	RayCollision collision;
	for (auto& i : gameObjects) {
		if (!i->GetBoundingVolume()) { 
			continue;
		}
		RayCollision thisCollision;
		if (CollisionDetection::RayIntersection(r, *i, thisCollision) && i != current) {
			if (!closestObject) {	
				closestCollision		= collision;
				closestCollision.node = i;
				return true;
			}
			else {
				if (thisCollision.rayDistance < collision.rayDistance) {
					thisCollision.node = i;
					collision = thisCollision;
				}
			}
		}
	}
	if (collision.node) {
		closestCollision		= collision;
		closestCollision.node	= collision.node;
		return true;
	}
	return false;
}


void GameWorld::AddConstraint(Constraint* c) {
	constraints.emplace_back(c);
}

void GameWorld::RemoveConstraint(Constraint* c, bool andDelete) {
	constraints.erase(std::remove(constraints.begin(), constraints.end(), c), constraints.end());
	if (andDelete) {
		delete c;
	}
}

void GameWorld::GetConstraintIterators(std::vector<Constraint*>::const_iterator& first, std::vector<Constraint*>::const_iterator& last) const {
	first	= constraints.begin();
	last	= constraints.end();
}


