#include "PhysicsSystem.h"
using namespace NCL;
using namespace CSC8503;


PhysicsSystem::PhysicsSystem(GameWorld& g) : gameWorld(g) {
	applyGravity = false;
	useBroadPhase = true;
	dTOffset = 0.0f;
	globalDamping = 0.995f;
	dampingFactor = 0.4;
	SetGravity(Vector3(0.0f, -9.8f, 0.0f));
	basicCollisionsTested = 0;
	totalCollisions = 0;
}

PhysicsSystem::~PhysicsSystem() {
}


void PhysicsSystem::Clear() {
	allCollisions.clear();
}

void PhysicsSystem::ClearDeletedCollisions() {
	for (std::set<CollisionDetection::CollisionInfo>::iterator i = allCollisions.begin(); i != allCollisions.end();) {
		if ((!i->a->IsActive() && !i->a->GetSelected()) || (!i->b->IsActive() && !i->b->GetSelected())) {
			
			if (!i->a->IsActive())
				i->a->SetIsSafeForDeletion(true);
			if (!i->b->IsActive())
				i->b->SetIsSafeForDeletion(true);
			i = allCollisions.erase(i);
		}
		else
			++i;
	}
}


const int   idealHZ = 120;
const float idealDT = 1.0f / idealHZ;


int realHZ = idealHZ;
float realDT = idealDT;

void PhysicsSystem::Update(float dt) {

	dTOffset += dt; 

	GameTimer t;
	t.GetTimeDeltaSeconds();

	if (useBroadPhase) {
		UpdateObjectAABBs();
	}

	while (dTOffset >= realDT) {
		IntegrateAccel(realDT); 
		if (useBroadPhase) {
			BroadPhase();
			NarrowPhase();
		}
		else
			BasicCollisionDetection();

		
		float constraintDt = realDT / (float)constraintIterationCount;
		for (int i = 0; i < constraintIterationCount; ++i) {
			UpdateConstraints(constraintDt);
		}
		IntegrateVelocity(realDT); 

		dTOffset -= realDT;
	}

	ClearForces();	

	UpdateCollisionList(); 

	t.Tick();
	float updateTime = t.GetTimeDeltaSeconds();

	
	if (updateTime > realDT) {
		realHZ /= 2;
		realDT *= 2;
		std::cout << "Dropping iteration count due to long physics time...(now " << realHZ << ")\n";
	}
	else if (dt * 2 < realDT) { 
		int temp = realHZ;
		realHZ *= 2;
		realDT /= 2;

		if (realHZ > idealHZ) {
			realHZ = idealHZ;
			realDT = idealDT;
		}
		if (temp != realHZ) {
			std::cout << "Raising iteration count due to short physics time...(now " << realHZ << ")\n";
		}
	}
}


void PhysicsSystem::UpdateCollisionList() {
	for (std::set<CollisionDetection::CollisionInfo>::iterator i = allCollisions.begin(); i != allCollisions.end(); ) {
		if ((*i).framesLeft == numCollisionFrames) {
			i->a->OnCollisionBegin(i->b);
			i->b->OnCollisionBegin(i->a);
		}
		(*i).framesLeft = (*i).framesLeft - 1;
		if ((*i).framesLeft < 0) {
			i->a->OnCollisionEnd(i->b);
			i->b->OnCollisionEnd(i->a);
			i = allCollisions.erase(i);
		}
		else {
			++i;
		}
	}
}

void PhysicsSystem::UpdateObjectAABBs() {
	gameWorld.OperateOnContents([](GameObject* g) {
		g->UpdateBroadphaseAABB();
		}
	);
}


void PhysicsSystem::BasicCollisionDetection() {
	basicCollisionsTested = 0;
	totalCollisions = 0;
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);
	for (auto i = first; i != last; ++i) {
		if ((*i)->GetPhysicsObject() == nullptr) {
			continue;
		}
		for (auto j = i + 1; j != last; ++j) {
			if ((*j)->GetPhysicsObject() == nullptr) {
				continue;
			}
			CollisionDetection::CollisionInfo info;
			basicCollisionsTested++;
			if (CollisionDetection::ObjectIntersection(*i, *j, info)) {
				if (!dynamic_cast<GrabPower*>(info.a) && !dynamic_cast<GrabPower*>(info.b))	// We dont want to resolve collisions with pickup volume
					ImpulseResolveCollision(*info.a, *info.b, info.point);
				info.framesLeft = numCollisionFrames;
				allCollisions.insert(info);
			}
		}
	}
	totalCollisions = allCollisions.size();
}


void PhysicsSystem::ImpulseResolveCollision(GameObject& a, GameObject& b, CollisionDetection::ContactPoint& p) const {
	PhysicsObject* physA = a.GetPhysicsObject();
	PhysicsObject* physB = b.GetPhysicsObject();
	Transform& transformA = a.GetTransform();
	Transform& transformB = b.GetTransform();
	float totalMass = physA->GetInverseMass() + physB->GetInverseMass();
	if (totalMass == 0) {
		return; 
	}
	
	if (physA->GetInverseMass() != 0.0f)
		transformA.SetPosition(transformA.GetPosition() - (p.normal * p.penetration * (physA->GetInverseMass() / totalMass)));
	if (physB->GetInverseMass() != 0.0f)
		transformB.SetPosition(transformB.GetPosition() + (p.normal * p.penetration * (physB->GetInverseMass() / totalMass)));

	
	Vector3 relativeA = p.localA;
	Vector3 relativeB = p.localB;
	Vector3 angVelocityA = Vector3::Cross(physA->GetAngularVelocity(), relativeA);
	Vector3 angVelocityB = Vector3::Cross(physB->GetAngularVelocity(), relativeB);
	Vector3 fullVelocityA = physA->GetLinearVelocity() + angVelocityA;
	Vector3 fullVelocityB = physB->GetLinearVelocity() + angVelocityB;
	Vector3 contactVelocity = fullVelocityB - fullVelocityA;
	float impulseForce = Vector3::Dot(contactVelocity, p.normal);
	
	Vector3 inertiaA = Vector3::Cross(physA->GetInertiaTensor() * Vector3::Cross(relativeA, p.normal), relativeA);
	Vector3 inertiaB = Vector3::Cross(physB->GetInertiaTensor() * Vector3::Cross(relativeB, p.normal), relativeB);
	float angularEffect = Vector3::Dot(inertiaA + inertiaB, p.normal);
	float cRestitution = physA->GetElasticity() * physB->GetElasticity(); 
	float j = (-(1.0f + cRestitution) * impulseForce) / (totalMass + angularEffect);
	Vector3 fullImpulse = p.normal * j;
	
	if (physA->GetInverseMass() != 0.0f) {
		physA->ApplyLinearImpulse(-fullImpulse);
		physA->ApplyAngularImpulse(Vector3::Cross(relativeA, -fullImpulse));
	}
	if (physB->GetInverseMass() != 0.0f) {
		physB->ApplyLinearImpulse(fullImpulse);
		physB->ApplyAngularImpulse(Vector3::Cross(relativeB, fullImpulse));
	}

	
	Vector3 t = contactVelocity - (p.normal * impulseForce);
	t.Normalise();
	float fricForce = Vector3::Dot(contactVelocity, t);
	Vector3 fricInertiaA = Vector3::Cross(physA->GetInertiaTensor() * Vector3::Cross(relativeA, t), relativeA);
	Vector3 fricInertiaB = Vector3::Cross(physB->GetInertiaTensor() * Vector3::Cross(relativeB, t), relativeB);
	float fricEffect = Vector3::Dot(fricInertiaA + fricInertiaB, t);
	float fricCoef = std::clamp(physA->GetFriction() * physB->GetFriction(), 0.0f, 1.0f);
	float jT = -(fricCoef * fricForce) / (totalMass + fricEffect);
	Vector3 fricImpulse = t * jT;
	
	if (physA->GetInverseMass() != 0.0f) {
		physA->ApplyLinearImpulse(-fricImpulse);
		physA->ApplyAngularImpulse(Vector3::Cross(relativeA, -fricImpulse));
	}
	if (physB->GetInverseMass() != 0.0f) {
		physB->ApplyLinearImpulse(fricImpulse);
		physB->ApplyAngularImpulse(Vector3::Cross(relativeB, fricImpulse));
	}

	
	float k = 0.005;
	float extension = 0 - p.penetration;
	float force = -k * extension;
	if (physA->GetInverseMass() != 0.0f)
		physA->AddForceAtLocalPosition(p.normal * -force, p.localA);
	if (physB->GetInverseMass() != 0.0f)
		physB->AddForceAtLocalPosition(p.normal * force, p.localB);
}


void PhysicsSystem::SpringTowardsPoint(ThisSpring* a) const {
	Vector3 extension = a->GetTransform().GetPosition() - a->GetRestPosition();
	Vector3 force = extension * -a->GetK();
	a->GetPhysicsObject()->ApplyLinearImpulse(force);
}


void PhysicsSystem::BroadPhase() {
	broadphaseCollisions.clear();
	QuadTree<GameObject*> tree(Vector2(2048, 2048), 7, 6);
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);
	for (auto i = first; i != last; ++i) {
		Vector3 halfSizes;
		if (!(*i)->GetBroadphaseAABB(halfSizes)) {
			continue;
		}
		Vector3 pos = (*i)->GetTransform().GetPosition();

		tree.Insert(*i, pos, halfSizes);
	}
	tree.OperateOnContents([&](std::list<QuadTreeEntry<GameObject*>>& data) {
		CollisionDetection::CollisionInfo info;
		for (auto i = data.begin(); i != data.end(); ++i) {
			for (auto j = std::next(i); j != data.end(); ++j) {
				info.a = min((*i).object, (*j).object);
				info.b = max((*i).object, (*j).object);

			
				if (info.a->GetPhysicsObject()->GetInverseMass() == 0.0f)
					info.a->GetPhysicsObject()->SetIsStatic(true);
				else
					info.a->GetPhysicsObject()->SetIsStatic(false);
				if (info.b->GetPhysicsObject()->GetInverseMass() == 0.0f)
					info.b->GetPhysicsObject()->SetIsStatic(true);
				else
					info.b->GetPhysicsObject()->SetIsStatic(false);

				if (info.a->GetPhysicsObject()->GetLinearVelocity().Length() == 0.0f && info.a->GetPhysicsObject()->GetAngularVelocity().Length() == 0.0f)
					info.a->GetPhysicsObject()->SetIsAsleep(true);
				else
					info.a->GetPhysicsObject()->SetIsAsleep(false);
				if (info.b->GetPhysicsObject()->GetLinearVelocity().Length() == 0.0f && info.b->GetPhysicsObject()->GetAngularVelocity().Length() == 0.0f)
					info.b->GetPhysicsObject()->SetIsAsleep(true);
				else
					info.b->GetPhysicsObject()->SetIsAsleep(false);

				bool bothStatic = info.a->GetPhysicsObject()->GetIsStatic() && info.b->GetPhysicsObject()->GetIsStatic();
				bool bothAsleep = info.a->GetPhysicsObject()->GetIsAsleep() && info.b->GetPhysicsObject()->GetIsAsleep();

				if (!(bothStatic || bothAsleep))
					broadphaseCollisions.insert(info);
			}
		}
		});
}


void PhysicsSystem::NarrowPhase() {
	totalCollisions = 0;
	for (std::set<CollisionDetection::CollisionInfo>::iterator i = broadphaseCollisions.begin(); i != broadphaseCollisions.end(); ++i) {
		CollisionDetection::CollisionInfo info = *i;
		if (CollisionDetection::ObjectIntersection(info.a, info.b, info)) {
			info.framesLeft = numCollisionFrames;
			if (!dynamic_cast<GrabPower*>(info.a) && !dynamic_cast<GrabPower*>(info.b))		// Don't resolve collisions with pickup objects
				ImpulseResolveCollision(*info.a, *info.b, info.point);
			allCollisions.insert(info);
			totalCollisions++;
		}
	}
	totalCollisions = allCollisions.size();
}


void PhysicsSystem::IntegrateAccel(float dt) {
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);
	for (auto i = first; i != last; ++i) {
		PhysicsObject* object = (*i)->GetPhysicsObject();
		if (object == nullptr) {
			continue; 
		}
		float inverseMass = object->GetInverseMass();
		Vector3 linearVel = object->GetLinearVelocity();
		Vector3 force = object->GetForce();
		Vector3 accel = force * inverseMass;
		if (applyGravity && inverseMass > 0) {
			accel += gravity * 3; 
		}
		linearVel += accel * dt; 
		object->SetLinearVelocity(linearVel);

		
		Vector3 torque = object->GetTorque();
		Vector3 angVel = object->GetAngularVelocity();
		object->UpdateInertiaTensor(); 

		Vector3 angAccel = object->GetInertiaTensor() * torque;
		angVel += angAccel * dt; 
		object->SetAngularVelocity(angVel);
	}
}


void PhysicsSystem::IntegrateVelocity(float dt) {
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);
	float frameLinearDamping = 1.0f - (dampingFactor * dt);
	for (auto i = first; i != last; ++i) {
		PhysicsObject* object = (*i)->GetPhysicsObject();
		if (object == nullptr) {
			continue;
		}
		Transform& transform = (*i)->GetTransform();
		// Position Stuff
		Vector3 position = transform.GetPosition();
		Vector3 linearVel = object->GetLinearVelocity();
		position += linearVel * dt;
		transform.SetPosition(position);
		// Linear Damping
		linearVel = linearVel * frameLinearDamping;
		object->SetLinearVelocity(linearVel);

		// Orientation Stuff
		Quaternion orientation = transform.GetOrientation();
		Vector3 angVel = dynamic_cast<SpinningTile*>((*i)) ? Vector3(0, 1, 0) : object->GetAngularVelocity();
		orientation = orientation + (Quaternion(angVel * dt * 0.5f, 0.0f) * orientation);
		orientation.Normalise();
		transform.SetOrientation(orientation);

		// Damp the angular velocity too
		float frameAngularDamping = 1.0f - (dampingFactor * dt);
		angVel = angVel * frameAngularDamping;
		object->SetAngularVelocity(angVel);

		/* Spring objects should have some force added towards their resting position */
		if (dynamic_cast<ThisSpring*>((*i)))
			SpringTowardsPoint((ThisSpring*)(*i));
	}
}

/* Once we're finished with a physics update, we have to
clear out any accumulated forces, ready to receive new
ones in the next 'game' frame. */
void PhysicsSystem::ClearForces() {
	gameWorld.OperateOnContents(
		[](GameObject* o) {
			o->GetPhysicsObject()->ClearForces(); 
		}
	);
}

/* As part of the final physics tutorials, we add in the ability
to constrain objects based on some extra calculation, allowing
us to model springs and ropes etc. */
void PhysicsSystem::UpdateConstraints(float dt) {
	std::vector<Constraint*>::const_iterator first;
	std::vector<Constraint*>::const_iterator last;
	gameWorld.GetConstraintIterators(first, last);
	for (auto i = first; i != last; ++i) {
		(*i)->UpdateConstraint(dt);
	}
}