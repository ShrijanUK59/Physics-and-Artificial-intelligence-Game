#include "TutorialGame.h"
using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame() {
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);
	physics = new PhysicsSystem(*world);
	physics->SetDampingFactor(0.3);
	forceMagnitude = 10.0f;
	useGravity = true;
	physics->UseGravity(useGravity);
	useBroadphase = true;
	physics->UseBroadphase(useBroadphase);
	inSelectionMode = false;
	reloadTime = 0.0f;
	timeOut = 0.0f;
	Debug::SetRenderer(renderer);
	player = nullptr;
	lockedOrientation = true;
	currentLevel = 0;
	currentlySelected = 1;
	finish = FinishType::INGAME;
	InitialiseAssets();
}

void TutorialGame::InitialiseAssets() {
	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh", &cubeMesh);
	loadFunc("sphere.msh", &sphereMesh);
	loadFunc("sphere.msh", &charMeshA);
	loadFunc("sphere.msh", &charMeshB);
	loadFunc("sphere.msh", &enemyMesh);
	loadFunc("coin.msh", &bonusMesh);
	loadFunc("capsule.msh", &capsuleMesh);

	portalTexture = (OGLTexture*)TextureLoader::LoadAPITexture("spike.png");
	jumpingtileTexture = (OGLTexture*)TextureLoader::LoadAPITexture("texture.png");
	woodTexture = (OGLTexture*)TextureLoader::LoadAPITexture("texture.png");
	endTexture = (OGLTexture*)TextureLoader::LoadAPITexture("end.png");
	baseTexture = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	projectionTexture = (OGLTexture*)TextureLoader::LoadAPITexture("dodge.png");
	groundTileTexture = (OGLTexture*)TextureLoader::LoadAPITexture("Floor img.png");
	mainmenuTexture = (OGLTexture*)TextureLoader::LoadAPITexture("menu.png");
	planeTexture = (OGLTexture*)TextureLoader::LoadAPITexture("cast.png");
	redbrickTexture = (OGLTexture*)TextureLoader::LoadAPITexture("redbrick.png");
	crossTexture = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	gametechShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame() {
	delete cubeMesh;
	delete sphereMesh;
	delete charMeshA;
	delete charMeshB;
	delete enemyMesh;
	delete bonusMesh;

	delete baseTexture;
	delete projectionTexture;
	delete groundTileTexture;
	delete portalTexture;
	delete jumpingtileTexture;
	delete crossTexture;
	delete woodTexture;
	delete endTexture;
	delete mainmenuTexture;
	delete gametechShader;

	delete physics;
	
	delete world;
}


void TutorialGame::UpdateMenu(float dt) {
	renderer->DrawString("Select Coursework Part A or Part B with Arrow Key", Vector2(0, 5), Debug::WHITE, 15.0f);
	currentlySelected == 1 ? renderer->DrawString("Coursework Part A", Vector2(0, 15), { 0,1,0,1 }) :
		renderer->DrawString("Coursework Part A", Vector2(0, 15), Debug::WHITE);
	currentlySelected == 3 ? renderer->DrawString("Coursework Part B", Vector2(0, 45), { 0,1,0,1 }) :
		renderer->DrawString("Coursework Part B", Vector2(0, 45), Debug::WHITE);
	renderer->DrawString("Difficulty", Vector2(0, 50), Debug::WHITE);
	currentlySelected == 3 ? renderer->DrawString("Easy", Vector2(35, 50), { 0,1,0,1 }) :
		renderer->DrawString("Easy", Vector2(35, 50), Debug::WHITE);
	currentlySelected == 4 ? renderer->DrawString("Medium", Vector2(45, 50), { 0,1,0,1 }) :
		renderer->DrawString("Medium", Vector2(45, 50), Debug::WHITE);
	currentlySelected == 5 ? renderer->DrawString("Hard", Vector2(60, 50), { 0,1,0,1 }) :
		renderer->DrawString("Hard", Vector2(60, 50), Debug::WHITE);
	renderer->DrawString("Press Escape to Quit", Vector2(10, 30));
	renderer->DrawString("Press Down to Change Difficulty", Vector2(10, 25));
	renderer->DrawString("Press Enter to Start", Vector2(10, 20));

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::UP) || Window::GetKeyboard()->KeyPressed(KeyboardKeys::DOWN)) {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::UP))
			currentlySelected = currentlySelected == 1 ? 5 : currentlySelected - 1;
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::DOWN))
			currentlySelected = currentlySelected == 5 ? 1 : currentlySelected + 1;
		switch (currentlySelected) {
		case 1:
			menuPlayers.at(1)->GetRenderObject()->SetColour({0.7,0.7,0.7,7});
			menuPlayers.at(0)->GetRenderObject()->SetColour(Vector4(0, 0.5, 1, 1));
			menuEnemies.at(0)->GetRenderObject()->SetColour({0.7,0.7,0.7,7});
			menuEnemies.at(1)->GetRenderObject()->SetColour({0.7,0.7,0.7,7});
			menuEnemies.at(2)->GetRenderObject()->SetColour({0.7,0.7,0.7,7});
			break;
		case 2:
			menuPlayers.at(0)->GetRenderObject()->SetColour({0.7,0.7,0.7,7});
			menuPlayers.at(1)->GetRenderObject()->SetColour(Vector4(0, 0.5, 1, 1));
			menuEnemies.at(0)->GetRenderObject()->SetColour({0.7,0.7,0.7,7});
			break;
		case 3:
			menuEnemies.at(0)->GetRenderObject()->SetColour((Vector4(1, 0, 0, 1)));
			menuEnemies.at(1)->GetRenderObject()->SetColour({0.7,0.7,0.7,7});
			break;
		case 4:
			menuEnemies.at(1)->GetRenderObject()->SetColour((Vector4(1, 0, 0, 1)));
			menuEnemies.at(2)->GetRenderObject()->SetColour({0.7,0.7,0.7,7});
			break;
		case 5:
			menuPlayers.at(0)->GetRenderObject()->SetColour({0.7,0.7,0.7,7});
			menuPlayers.at(1)->GetRenderObject()->SetColour(Vector4(0, 0.5, 1, 1));
			menuEnemies.at(0)->GetRenderObject()->SetColour((Vector4(1, 0, 0, 1)));
			menuEnemies.at(1)->GetRenderObject()->SetColour((Vector4(1, 0, 0, 1)));
			menuEnemies.at(2)->GetRenderObject()->SetColour((Vector4(1, 0, 0, 1)));
			break;
		}
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN)) {
		currentLevel = currentlySelected >= 2 ? 2 : 1;
		InitWorld();		
	}
}


void TutorialGame::UpdateLevel(float dt) {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (dynamic_cast<GameObjectEnemy*>(selectionObject))
			((GameObjectEnemy*)selectionObject)->SetDisplayDirection(inSelectionMode);
		if (dynamic_cast<EnemyBallBehaviourTree*>(selectionObject))
			((EnemyBallBehaviourTree*)selectionObject)->SetDisplayRoute(inSelectionMode);
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}

		renderer->DrawString("Press Q for Debug mode", Vector2(35, 10), Debug::WHITE, textSize);
	if (inSelectionMode) {
		UpdateKeys();
		SelectObject();
		DrawDebugInfo();
		world->ShowFacing();
	}
	else {
		if (player) {
			renderer->DrawString("Time (Seconds):" + std::to_string(player->GetScore()), Vector2(0, 10), Debug::WHITE, textSize);
			player->Update(dt);
		}
	}

	switch (camState) {
	case CameraState::THIRDPERSON:
		renderer->DrawString("Press 1 Topdown Camera", Vector2(35, 20), Debug::WHITE, textSize);
		break;
	case CameraState::TOPDOWN:
		renderer->DrawString("Press 1 Normal Camera", Vector2(35, 20), Debug::WHITE, textSize);
		break;
	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::NUM1)) {
		switch (camState) {
		case CameraState::FREE:
			camState = currentLevel == 1 ? CameraState::GLOBAL1 : CameraState::GLOBAL2;
			break;
		case CameraState::GLOBAL1:
			InitCamera();
			camState = CameraState::FREE;
			break;
		case CameraState::GLOBAL2:
			InitCamera();
			camState = CameraState::FREE;
			break;
		}
		world->GetMainCamera()->SetState(camState);
	}

	physics->Update(dt);		

	if (lockedObject) {
		world->GetMainCamera()->UpdateCameraWithObject(dt, lockedObject);
		if (lockedOrientation)
			lockedObject->GetTransform().SetOrientation(Matrix4::Rotation(world->GetMainCamera()->GetYaw(), { 0, 1, 0 }));
	}
	else if (!inSelectionMode || camState == CameraState::GLOBAL1 || camState == CameraState::GLOBAL2)
		world->GetMainCamera()->UpdateCamera(dt);

	renderer->DrawString("Press Escape to Go Back To Menu", Vector2(25, 5), Debug::WHITE, textSize);
	renderer->DrawString("Press P to Pause", Vector2(39, 15), Debug::WHITE, textSize);

	currentLevel == 1 ? UpdateLevel1(dt) : UpdateLevel2(dt);		

	lockedObject ? LockedObjectMovement(dt) : DebugObjectMovement();
	CheckFinished(dt);		
}

void TutorialGame::DrawDebugInfo() {
	useGravity ? renderer->DrawString("Press G for Gravity:On", Vector2(0, 20), Debug::WHITE, textSize) :
		renderer->DrawString("Press G for Gravity:Off", Vector2(0, 20), Debug::WHITE, textSize);
	

	
	if (lockedObject) {
		renderer->DrawString("Press L to Lock or Unlock Object", Vector2(0, 30), Debug::WHITE, textSize);
	}

	if (selectionObject) {
		if (dynamic_cast<ObjectStateGame*>(selectionObject)) {
			renderer->DrawString("State:" + ((ObjectStateGame*)selectionObject)->StateToString(), Vector2(0, 35), Debug::WHITE, textSize);
			selectionObject->GetPowerUpTimer() > 0.0f ? renderer->DrawString("Power Activity", Vector2(0, 30), Debug::WHITE, textSize) :
				renderer->DrawString("Power Not Gained", Vector2(0, 30), Debug::WHITE, textSize);
		}
		else if (dynamic_cast<EnemyBallBehaviourTree*>(selectionObject)) {
			renderer->DrawString("Current Behaviour:" + ((EnemyBallBehaviourTree*)selectionObject)->BehaviourToString(), Vector2(0, 60), Debug::WHITE, textSize);
			selectionObject->GetPowerUpTimer() > 0.0f ? renderer->DrawString("Power Activity", Vector2(0, 65), Debug::WHITE, textSize) :
				renderer->DrawString("Power Not Gained", Vector2(0, 40), Debug::WHITE, textSize);
		}
		
		renderer->DrawString("Selected Object:" + selectionObject->GetName(), Vector2(0, 40), Debug::WHITE, textSize);
		renderer->DrawString("Position:" + selectionObject->GetTransform().GetPosition().ToString(), Vector2(0, 45), Debug::WHITE, textSize);
		renderer->DrawString("Orientation:" + selectionObject->GetTransform().GetOrientation().ToEuler().ToString(), Vector2(0, 50), Debug::WHITE, textSize);
		renderer->DrawString("Linear Velocity:" + selectionObject->GetPhysicsObject()->GetLinearVelocity().ToString(), Vector2(0, 55), Debug::WHITE, textSize);
		
	}
}

void TutorialGame::CheckFinished(float dt) {
	if (player && !inSelectionMode) {
		if ((player->GetScore() <= 0 || player->GetFinished()) && finish == FinishType::INGAME) {
			if (player->GetScore() <= 0)
				finish = FinishType::TIMEOUT;
			else if (player->GetFinished())
				finish = FinishType::WIN;
		}
	}

	for (auto& e : enemies) {
		if (e->GetFinished() && finish == FinishType::INGAME)
			finish = FinishType::LOSE;		
	}

	switch (finish) {
	case FinishType::INGAME:
		if (player && !inSelectionMode)
			player->DecreaseScore(dt);
		break;
	case FinishType::TIMEOUT:
		renderer->DrawString("Game Over ! Back To Menu", Vector2(0, 30), Vector4(1, 0, 0, 1), 25.0f);
		break;
	case FinishType::WIN:
		renderer->DrawString("Winner !", Vector2(34, 35), Vector4(0, 1, 0, 1), 25.0f);
		renderer->DrawString("Final Score: " + std::to_string(player->GetScore()), Vector2(34, 40), Vector4(1, 1, 0, 1));
		lockedOrientation = false;
		break;
	case FinishType::LOSE:
		renderer->DrawString("Game Over ! Back To Menu", Vector2(0, 30), Vector4(1, 0, 0, 1), 25.0f);
		if (player)
			renderer->DrawString("Lost" , Vector2(0, 35), Vector4(1, 1, 0, 1));
		break;
	}

	if (finish != FinishType::INGAME) {
		timeOut += dt;
		renderer->DrawString("Back to Menu Screen", Vector2(10, 50));
	}
}

void TutorialGame::UpdateLevel1(float dt) {
	for (auto& p : platforms)
		p->Update(dt);


	reloadTime += dt;
	if (reloadTime > 2.0f) {
		FireObjects();
		reloadTime = 0.0f;
	}

	for (auto& o : projectiles)
		o->StepTimeAlive(dt);		

	if (player) {
		switch ((int)player->GetTransform().GetPosition().z) {
		case 0:
			player->SetSpawnPos({ 0, 10, 50 });
			break;
		case -10:
			camState = CameraState::TOPDOWN;
			world->GetMainCamera()->SetState(camState);
			break;
		case -260:
			player->SetSpawnPos({ 0, 10, -260 });
			break;
		case -415:
			camState = CameraState::TOPDOWN;
			world->GetMainCamera()->SetState(camState);
			break;
		case -590:
			player->SetSpawnPos({ 0, 10, -590 });
			break;
		case -815:
			player->SetSpawnPos({ 0, 10, -815 });
			camState = CameraState::TOPDOWN;
			world->GetMainCamera()->SetState(camState);
			break;
		case -970:
			if ((int)player->GetTransform().GetPosition().x == -30 && (int)player->GetTransform().GetPosition().y > 20)
				player->SetSpawnPos({ -30, 50, -970 });
			else if ((int)player->GetTransform().GetPosition().x >= 120 && (int)player->GetTransform().GetPosition().y > 20) {
				camState = CameraState::TOPDOWN;
				world->GetMainCamera()->SetState(camState);
			}
			else {
				camState = CameraState::TOPDOWN;
				world->GetMainCamera()->SetState(camState);
			}
			break;
		}
	}
}


void TutorialGame::FireObjects() {
	SphereObject* sphere = new SphereObject;
	

	
}


void TutorialGame::UpdateLevel2(float dt) {
	if (player)
		player->SetSpawnPos({ 0, 10, 0 });
	for (auto& e : enemies) {
		e->Update(dt);
		currentEnemy = e;
		EnemyRaycast();
	}
}


void TutorialGame::EnemyRaycast() {
	float fov = -1, numRay = 55;
	if (dynamic_cast<GameObjectEnemy*>(currentEnemy)) {
		if (((GameObjectEnemy*)currentEnemy)->GetRayTime() < 0.0f) {
			for (int i = 0; i < numRay; ++i) {
				Ray ray(((GameObjectEnemy*)currentEnemy)->GetTransform().GetPosition(),
					((GameObjectEnemy*)currentEnemy)->GetTransform().GetOrientation() * Vector3(fov, 0, -1));
				RayCollision closestCollision;
				if (world->Raycast(ray, closestCollision, currentEnemy, true) && closestCollision.rayDistance < 80.0f) {
					
					if (dynamic_cast<PlayerBall*>((GameObject*)closestCollision.node) ||
						dynamic_cast<GrabPower*>((GameObject*)closestCollision.node)) {
						((GameObjectEnemy*)currentEnemy)->AddFollowObject((GameObject*)closestCollision.node);
						((GameObjectEnemy*)currentEnemy)->SetFollowTimeOut(55.0f);
					}
				}
				if (currentEnemy == selectionObject && inSelectionMode)
					closestCollision.node ? Debug::DrawLine(ray.GetPosition(), closestCollision.collidedAt, Debug::GREEN) :
					Debug::DrawLine(ray.GetPosition(), ray.GetPosition() + (ray.GetDirection() * 80), Debug::RED);
				fov += 2 / (numRay - 1);
			}
			((GameObjectEnemy*)currentEnemy)->SetRayTime(0.25f);
		}
	}
	else if (dynamic_cast<EnemyBallBehaviourTree*>(currentEnemy)) {
		if (((EnemyBallBehaviourTree*)currentEnemy)->GetRayTime() < 0.0f && ((EnemyBallBehaviourTree*)currentEnemy)->GetRayCast()) {
			for (int i = 0; i < numRay; ++i) {
				Ray ray(((EnemyBallBehaviourTree*)currentEnemy)->GetTransform().GetPosition(),
					((EnemyBallBehaviourTree*)currentEnemy)->GetTransform().GetOrientation() * Vector3(fov, 0, -1));
				RayCollision closestCollision;
				if (world->Raycast(ray, closestCollision, currentEnemy, true) && closestCollision.rayDistance < 80.0f) {
					
					if (dynamic_cast<PlayerBall*>((GameObject*)closestCollision.node) ||
						dynamic_cast<GrabPower*>((GameObject*)closestCollision.node)) {
						((EnemyBallBehaviourTree*)currentEnemy)->SetFoundObject((GameObject*)closestCollision.node);
					}
				}
				if (currentEnemy == selectionObject && inSelectionMode)
					closestCollision.node ? Debug::DrawLine(ray.GetPosition(), closestCollision.collidedAt, Debug::GREEN) :
					Debug::DrawLine(ray.GetPosition(), ray.GetPosition() + (ray.GetDirection() * 80), Debug::RED);
				fov += 2 / (numRay - 1);
			}
			((EnemyBallBehaviourTree*)currentEnemy)->SetRayTime(0.25f);
		}
	}
}


void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; 
		physics->UseGravity(useGravity);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::B)) {
		useBroadphase = !useBroadphase;
		physics->UseBroadphase(useBroadphase);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::I))
		physics->IncreaseIterationCount();
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::O))
		physics->DecreaseIterationCount();


	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1))
		world->ShuffleObjects(!world->GetShuffleObjects());
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2))
		world->ShuffleConstraints(!world->GetShuffleConstraints());

	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 10.0f;
}


void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.5f);
	world->GetMainCamera()->SetFarPlane(1600.0f);
	world->GetMainCamera()->SetPosition(Vector3(0, 50, 80));
	world->GetMainCamera()->SetYaw(0);
	world->GetMainCamera()->SetPitch(0);
	lockedObject = nullptr;
}


void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();
	InitFloors(currentLevel);
	InitGameExamples(currentLevel);
	InitGameObstacles(currentLevel);
}


void TutorialGame::InitFloors(int level) {
	switch (level) {
	case 0:
		AddFloorToWorld(new CollisionCrash, Vector3(0, 50, 0), Vector3(80, 40, 1))->GetRenderObject()->SetDefaultTexture(mainmenuTexture);
		break;
	case 1:
		AddFloorToWorld(new GroundObject, Vector3(0, -60, -400), Vector3(500, 1, 1000));
		AddFloorToWorld(new CollisionCrash, Vector3(0, 0, 50), Vector3(25, 1, 25));
		AddFloorToWorld(new CollisionCrash, Vector3(0, 0, 0), Vector3(25, 1, 25));
		AddFloorToWorld(new CollisionCrash, Vector3(0, 10, -107), Vector3(25, 1, 40), Matrix4::Rotation(15, { 1, 0, 0 }));
		AddFloorToWorld(new CollisionCrash, Vector3(0, 20.4, -150.2), Vector3(25, 1, 5));
		AddFloorToWorld(new CollisionCrash, Vector3(0, 10, -175), Vector3(25, 1, 23), Matrix4::Rotation(-27, { 1, 0, 0 }));
		AddFloorToWorld(new CollisionCrash, Vector3(30, 0, -205), Vector3(100, 1, 10));
		AddFloorToWorld(new CollisionCrash, Vector3(-60, 0, -215), Vector3(10, 1, 10));
		AddFloorToWorld(new CollisionCrash, Vector3(90, 0, -225), Vector3(10, 1, 10));
		AddFloorToWorld(new CollisionCrash, Vector3(30, 0, -235), Vector3(100, 1, 10));
		AddFloorToWorld(new CollisionCrash, Vector3(90, 0, -265), Vector3(10, 1, 10));
		AddFloorToWorld(new CollisionCrash, Vector3(20, 0, -225), Vector3(5, 1, 5));
		AddFloorToWorld(new CollisionCrash, Vector3(0, -30, -315), Vector3(10, 1, 10));
		AddFloorToWorld(new CollisionCrash, Vector3(0, -30, -395), Vector3(10, 1, 10));
		AddFloorToWorld(new CollisionCrash, Vector3(0, 0, -445), Vector3(10, 1, 10));
		AddFloorToWorld(new CollisionCrash, Vector3(30, 20, -475), Vector3(30, 41, 15), Matrix4::Rotation(30, { 0, 1, 0 }));
		AddFloorToWorld(new CollisionCrash, Vector3(30, 20, -515), Vector3(15, 41, 60), Matrix4::Rotation(75, { 0, 1, 0 }));
		AddFloorToWorld(new CollisionCrash, Vector3(30, 20, -555), Vector3(30, 41, 15), Matrix4::Rotation(150, { 0, -1, 0 }));
		AddFloorToWorld(new CollisionCrash, Vector3(0, 0, -605), Vector3(10, 1, 30));
		AddFloorToWorld(new CollisionCrash, Vector3(0, 0, -645), Vector3(80, 1, 10));
		AddFloorToWorld(new CollisionCrash, Vector3(-90, 0, -735), Vector3(10, 1, 100));
		AddFloorToWorld(new CollisionCrash, Vector3(90, 0, -735), Vector3(10, 1, 100));
		AddFloorToWorld(new CollisionCrash, Vector3(0, 0, -825), Vector3(80, 1, 10));
		AddFloorToWorld(new CollisionCrash, Vector3(0, 0, -865), Vector3(5, 1, 30));
		AddFloorToWorld(new CollisionCrash, Vector3(-25, 0, -900), Vector3(30, 1, 5));
		AddFloorToWorld(new CollisionCrash, Vector3(-60, 0, -915), Vector3(5, 1, 20));
		AddFloorToWorld(new CollisionCrash, Vector3(-60, -20, -1005), Vector3(5, 1, 25));
		AddFloorToWorld(new CollisionCrash, Vector3(-59, -20, -1055), Vector3(25, 1, 25));
		AddFloorToWorld(new CollisionCrash, Vector3(103, 26.5, -990), Vector3(20, 1, 5), Matrix4::Rotation(20, { 0, 0, 1 }));
		AddFloorToWorld(new CollisionCrash, Vector3(126.2, 33.3, -995), Vector3(5, 1, 35));
		AddFloorToWorld(new CollisionCrash, Vector3(155, 8.5, -1025), Vector3(35, 1, 5), Matrix4::Rotation(45, { 0, 0, -1 }));
		AddFloorToWorld(new CollisionCrash, Vector3(155, 8.5, -965), Vector3(35, 1, 5), Matrix4::Rotation(45, { 0, 0, -1 }));
		AddFloorToWorld(new GameEnd, Vector3(204, -50, -995), Vector3(39, 1, 38));
		AddFloorToWorld(new TileGround, Vector3(204, -52, -995), Vector3(41, 1, 40));
		break;

	case 2:
		AddFloorToWorld(new GroundObject, Vector3(0, -60, -500), Vector3(800, 1, 800));
		AddFloorToWorld(new TileGround, Vector3(0, 0, 10), Vector3(30, 1, 40));
		AddFloorToWorld(new TileGround, Vector3(0, 0, 10), Vector3(40, 1, 50));
		AddFloorToWorld(new TileGround, Vector3(0, 0, -240), Vector3(210, 1, 210));
		CreateMaze();
		AddFloorToWorld(new GameEnd, Vector3(50, 1, -265), Vector3(10, 1, 10));
		break;
	}
}

void TutorialGame::CreateMaze() {
	NavigationGrid grid("MazePath.txt");
	Vector3 offset(220, -20, 460), nodePos, objectPos, icePos;
	float xSize = 10, zSize = 10, iceXSize = 10, iceZsize = 10;

	for (int y = 0; y < 23; ++y) {
		nodePos = grid.GetNodes()[23 * y].position;
		xSize = 10;
		iceXSize = 10;
		for (int x = 0; x < 23; ++x) {
			objectPos = grid.GetNodes()[(23 * y) + x + 1].position;


			if (grid.GetNodes()[(23 * y) + x].type == POWERUP_NODE)
				AddPickupToWorld(new PowerupObject, objectPos - offset + Vector3(-20, -16, 65));

			if (grid.GetNodes()[(23 * y) + x].type == WALL_NODE) {
				if ((grid.GetNodes()[(23 * y) + x + 1].type != WALL_NODE || x == 22) && xSize > zSize)
					AddFloorToWorld(new TileGround, nodePos - offset, { xSize, 20, zSize })->GetRenderObject()->SetDefaultTexture(redbrickTexture);
				else {
					nodePos.x += 10;
					xSize += 10;
				}
			}
			else {
				xSize = 10;
				nodePos = grid.GetNodes()[(23 * y) + x + 1].position;
			}
			if (grid.GetNodes()[(23 * y) + x].type == ICE_NODE) {
				if ((grid.GetNodes()[(23 * y) + x + 1].type != ICE_NODE || x == 22) && iceXSize > iceZsize)
					AddFloorToWorld(new CrossObject, icePos - offset - Vector3(0, 19, 0), Vector3(iceXSize, 0.1, iceZsize));
				else {
					icePos.x += 10;
					iceXSize += 10;
				}
			}
			else {
				iceXSize = 10;
				icePos = grid.GetNodes()[(23 * y) + x + 1].position;
			}
		}
	}

	xSize = 10;
	iceXSize = 10;
	for (int x = 0; x < 23; ++x) {
		nodePos = grid.GetNodes()[x].position;
		icePos = grid.GetNodes()[x].position;
		zSize = 10;
		for (int y = 0; y < 23; ++y) {
			if (grid.GetNodes()[(23 * y) + x].type == WALL_NODE) {
				if ((grid.GetNodes()[(23 * y) + x + 23].type != WALL_NODE || y == 22) && zSize > xSize)
					AddFloorToWorld(new TileGround, nodePos - offset, { xSize, 20, zSize })->GetRenderObject()->SetDefaultTexture(redbrickTexture);
				else {
					nodePos.z += 10;
					zSize += 10;
				}
			}
			else {
				zSize = 10;
				nodePos = grid.GetNodes()[(23 * y) + x + 23].position;
			}
			if (grid.GetNodes()[(23 * y) + x].type == ICE_NODE) {
				if ((grid.GetNodes()[(23 * y) + x + 23].type != ICE_NODE || y == 22) && iceZsize >= iceXSize)
					AddFloorToWorld(new CrossObject, icePos - offset - Vector3(0, 19, 0), Vector3(iceXSize, 0.1, iceZsize));
				else {
					icePos.z += 10;
					iceZsize += 10;
				}
			}
			else {
				iceZsize = 10;
				icePos = grid.GetNodes()[(23 * y) + x + 23].position;
			}
		}
	}
}


void TutorialGame::InitGameExamples(int level) {
	vector<Vector3> positions = { Vector3(-59, -15, -1035), Vector3(-75, -15, -1055),  Vector3(-59, -15, -1075),
		Vector3(-43, -15, -1055), Vector3(-59, -15, -1035) };		
	switch (level) {
	case 0:
		menuPlayers.push_back(AddPlayerToWorld(new PlayerBall, Vector3(-20, 55, 70)));
		menuPlayers.push_back(AddPlayerToWorld(new PlayerBall, Vector3(-20, 46, 80)));
		menuEnemies.push_back(AddEnemyToWorld(new EnemyObject, { -15, 46, 90 }));
		menuEnemies.push_back(AddEnemyToWorld(new EnemyObject, { -10, 46, 100 }));
		menuEnemies.push_back(AddEnemyToWorld(new EnemyObject, { -5, 46, 110 }));
		for (auto& e : menuEnemies) {
			e->GetRenderObject()->SetColour({0.7,0.7,0.7,7});
			e->GetTransform().SetOrientation(Matrix4::Rotation(180, Vector3(0, 1, 0)));
		}
		for (int i = 0; i < menuPlayers.size(); ++i) {
			GameObject* o = menuPlayers.at(i);
			if (i == 1)
				o->GetRenderObject()->SetColour({0.7,0.7,0.7,7});
			o->GetTransform().SetOrientation(Matrix4::Rotation(180, Vector3(0, 1, 0)));
		}
		break;
	case 1:
		InitPlayer();
		platforms.push_back((MovementFloor*)AddFloorToWorld(
			new MovementFloor(Vector3(-12.5, 50, -49), Vector3(12.5, 50, -49)), Vector3(-12.5, 34, -49), { 12.5,41,24 }));
		platforms.push_back((MovementFloor*)AddFloorToWorld(
			new MovementFloor(Vector3(-32.5, 40, -130), Vector3(12.5, 40, -130)), Vector3(-32.5, 34, -130), { 35,41,5 }));
		platforms.push_back((MovementFloor*)AddFloorToWorld(
			new MovementFloor(Vector3(-45, -20, -990), Vector3(-45, 70, -990)), Vector3(-45, -20, -990), { 10,1,10 }));

		
		AddPickupToWorld(new PowerupObject, Vector3(0, 93, -15));
		AddPickupToWorld(new PowerupObject, Vector3(-90, 93, -225));
		AddPickupToWorld(new PowerupObject, Vector3(90, 93, -225));
		AddPickupToWorld(new PowerupObject, Vector3(0, 93, -225));
		AddPickupToWorld(new PowerupObject, Vector3(0, 93, -375));
		AddPickupToWorld(new PowerupObject, Vector3(-59, 93, -1055));
		AddPickupToWorld(new PowerupObject, Vector3(0, 93, -55));
		AddPickupToWorld(new PowerupObject, Vector3(-90, 93, -155));
		AddPickupToWorld(new PowerupObject, Vector3(90, 93, -195));
		AddPickupToWorld(new PowerupObject, Vector3(0, 93, -305));
		AddPickupToWorld(new PowerupObject, Vector3(0, 93, -410));
		AddPickupToWorld(new PowerupObject, Vector3(-59, 93, -1055));
		AddPickupToWorld(new PowerupObject, Vector3(-61, 93, -1057));
		AddPickupToWorld(new PowerupObject, Vector3(-60, 93, -990));
		AddPickupToWorld(new PowerupObject, Vector3(0, 93, -35));
		AddPickupToWorld(new PowerupObject, Vector3(-90, 93, -255));
		AddPickupToWorld(new PowerupObject, Vector3(90, 93, -295));
		AddPickupToWorld(new PowerupObject, Vector3(0, 93, -325));
		AddPickupToWorld(new PowerupObject, Vector3(0, 93, -395));
		AddPickupToWorld(new PowerupObject, Vector3(-59, 93, -1025));
		AddPickupToWorld(new PowerupObject, Vector3(0, 93, -95));
		AddPickupToWorld(new PowerupObject, Vector3(-90, 93, -125));
		AddPickupToWorld(new PowerupObject, Vector3(90, 93, -395));
		AddPickupToWorld(new PowerupObject, Vector3(0, 93, -335));
		AddPickupToWorld(new PowerupObject, Vector3(0, 93, -510));
		AddPickupToWorld(new PowerupObject, Vector3(-59, 93, -935));
		AddPickupToWorld(new PowerupObject, Vector3(-61, 93, -949));
		AddPickupToWorld(new PowerupObject, Vector3(-60, 93, -970));
		break;
	case 2:
		InitPlayer();
		int numEnemies = currentlySelected - 2;
		if (numEnemies >= 1)
			enemies.push_back((EvilBallEnemy*)AddEnemyToWorld(new EvilBallEnemy(true), Vector3(-5, 10, 0)));
		if (numEnemies >= 2)
			enemies.push_back((EvilBallEnemy*)AddEnemyToWorld(new EvilBallEnemy(false), Vector3(0, 10, 0)));
		if (numEnemies == 3)
			enemies.push_back((EvilBallEnemy*)AddEnemyToWorld(new EvilBallEnemy(false), Vector3(5, 10, 0)));

		break;
	}
}

void TutorialGame::InitPlayer() {
	Vector3 position = currentLevel == 1 ? Vector3(0, 10, 50) : Vector3(0, 15, 30);
	player = (PlayerBall*)AddPlayerToWorld(new PlayerBall, position);
	lockedObject = player;
	selectionObject = player;
	selectionObject->SetSelected(true);
	camState = CameraState::THIRDPERSON;
	world->GetMainCamera()->SetState(camState);
	world->GetMainCamera()->SetPosition(player->GetTransform().GetPosition());
}

void TutorialGame::InitGameObstacles(int level) {
	switch (level) {
	case 1:
		AddCubeToWorld(new SpinningTile, Vector3(0, 139, -505), Vector3(1, 1, 100));
		AddCubeToWorld(new SpinningTile, Vector3(0, 139, -705), Vector3(1, 1, 100));
		AddCubeToWorld(new SpinningTile, Vector3(0, 139, -905), Vector3(1, 1, 100));
		AddCubeToWorld(new ThisSpring(Vector3(0, 5, -890), 0.005f), Vector3(0, 5, -890), Vector3(4, 4, 1));
		AddCubeToWorld(new ThisSpring(Vector3(-50, 5, -900), 0.005f), Vector3(-50, 5, -900), Vector3(1, 4, 4));
		AddCubeToWorld(new ThisSpring(Vector3(-60, -15, -955), 0.01f), Vector3(-60, -15, -955), Vector3(10, 1, 15));
		AddBridgeToWorld(Vector3(-30, 20, -990));
		break;
	}
}


void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(new SphereObject, position, radius);
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(new CubeObject, position, cubeDims);
		}
	}
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			int type = rand() % 3;
			switch (type) {
			case 0:
				(rand() % 2) ? AddCubeToWorld(new CubeObject, position, cubeDims * Vector3(1, 4, 1)) : AddCubeToWorld(new CubeObject, position, cubeDims);
				break;
			case 1:
				(rand() % 2) ? AddSphereToWorld(new SphereObject, position, sphereRadius / 2) : AddSphereToWorld(new SphereObject, position, sphereRadius);
				break;
			case 2:
				(rand() % 2) ? AddCapsuleToWorld(new CapsuleObject, position, 3.0f, sphereRadius * 1.5) : AddCapsuleToWorld(new CapsuleObject, position, 2.0f, sphereRadius);
				break;
			}
		}
	}
}

GameObject* TutorialGame::AddFloorToWorld(GameObject* floor, const Vector3& position, const Vector3& size, const Matrix4& orientation) {
	if (Vector3(1, 1, 1) != orientation * Vector3(1, 1, 1)) {
		OBBVolume* volume = new OBBVolume(size);
		floor->SetBoundingVolume((CollisionVolume*)volume);
	}
	else {
		AABBVolume* volume = new AABBVolume(size);
		floor->SetBoundingVolume((CollisionVolume*)volume);
	}

	floor->GetTransform().SetScale(size * 2).SetPosition(position).SetOrientation(orientation);
	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, groundTileTexture, gametechShader));

	if (dynamic_cast<GroundObject*>(floor))
		floor->GetRenderObject()->SetDefaultTexture(portalTexture);
	else if (dynamic_cast<CrossObject*>(floor))
		floor->GetRenderObject()->SetDefaultTexture(crossTexture);
	else if (dynamic_cast<CollisionCrash*>(floor))
		floor->GetRenderObject()->SetDefaultTexture(jumpingtileTexture);
	else if (dynamic_cast<GameEnd*>(floor))
		floor->GetRenderObject()->SetDefaultTexture(endTexture);

	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));
	floor->GetPhysicsObject()->InitCubeInertia();
	world->AddGameObject(floor);
	return floor;
}

GameObject* TutorialGame::AddSphereToWorld(GameObject* sphere, const Vector3& position, float radius) {
	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);
	sphere->GetTransform().SetScale(sphereSize).SetPosition(position);
	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, projectionTexture, gametechShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));
	sphere->GetPhysicsObject()->InitSphereInertia(false);
	world->AddGameObject(sphere);
	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(GameObject* cube, const Vector3& position, Vector3 dimensions) {
	CollisionVolume* volume;
	dynamic_cast<SpinningTile*>(cube) ? volume = new OBBVolume(dimensions) : volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);
	cube->GetTransform().SetPosition(position).SetScale(dimensions * 2);
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, projectionTexture, gametechShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));
	cube->GetPhysicsObject()->InitCubeInertia();
	world->AddGameObject(cube);
	return cube;
}

GameObject* TutorialGame::AddCapsuleToWorld(GameObject* capsule, const Vector3& position, float halfHeight, float radius) {
	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);
	capsule->GetTransform().SetScale(Vector3(radius * 2, halfHeight, radius * 2)).SetPosition(position);
	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, projectionTexture, gametechShader));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));
	capsule->GetPhysicsObject()->InitCapsuleInertia();
	world->AddGameObject(capsule);
	return capsule;
}

void TutorialGame::AddBridgeToWorld(Vector3 startPos) {
	Vector3 cubeSize(2.5, 0.5, 5);
	Vector3 baseSize(4, 1, 5);
	float bridgeWidth = 110;
	float invCubeMass = 5; 
	int numLinks = 15;
	float maxDistance = (bridgeWidth / (numLinks + 2)); 
	float cubeDistance = (bridgeWidth / (numLinks + 2));
	GameObject* start = AddFloorToWorld(new TileGround, startPos + Vector3(0, 0, 0), baseSize);
	GameObject* end = AddFloorToWorld(new TileGround, startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), baseSize);
	GameObject* previous = start;
	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(new CubeObject, startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize);
		block->GetRenderObject()->SetDefaultTexture(woodTexture);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}
	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

GameObject* TutorialGame::AddPlayerToWorld(GameObject* p, const Vector3& position) {
	float meshSize = 3.0f;
	CapsuleVolume* volume = new CapsuleVolume(meshSize * 0.85, 2.0f);
	p->SetBoundingVolume((CollisionVolume*)volume);
	p->GetTransform().SetScale(Vector3(meshSize, meshSize, meshSize)).SetPosition(position);
	(rand() % 2) ? p->SetRenderObject(new RenderObject(&p->GetTransform(), charMeshA, planeTexture, gametechShader)) :
		p->SetRenderObject(new RenderObject(&p->GetTransform(), charMeshB, planeTexture, gametechShader));
	p->GetRenderObject()->SetColour(Vector4(0, 0.5, 1, 1));
	p->SetPhysicsObject(new PhysicsObject(&p->GetTransform(), p->GetBoundingVolume()));
	p->GetPhysicsObject()->InitCapsuleInertia();
	world->AddGameObject(p);
	return p;
}

GameObject* TutorialGame::AddEnemyToWorld(GameObject* e, const Vector3& position) {
	float meshSize = 3.0f;
	CapsuleVolume* volume = new CapsuleVolume(meshSize * 0.85, 2.0f);
	e->SetBoundingVolume((CollisionVolume*)volume);
	e->GetTransform().SetScale(Vector3(meshSize, meshSize, meshSize)).SetPosition(position);
	e->SetRenderObject(new RenderObject(&e->GetTransform(), enemyMesh, planeTexture, gametechShader));
	e->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
	e->SetPhysicsObject(new PhysicsObject(&e->GetTransform(), e->GetBoundingVolume()));
	e->GetPhysicsObject()->InitCapsuleInertia();
	world->AddGameObject(e);
	return e;
}

GameObject* TutorialGame::AddPickupToWorld(GameObject* p, const Vector3& position) {
	SphereVolume* volume = new SphereVolume(1.25f);
	p->SetBoundingVolume((CollisionVolume*)volume);
	p->GetTransform().SetScale(Vector3(0.25, 0.25, 0.25)).SetPosition(position);
	p->SetRenderObject(new RenderObject(&p->GetTransform(), bonusMesh, baseTexture, gametechShader));
	dynamic_cast<PowerupObject*>(p) ? p->GetRenderObject()->SetColour(Debug::MAGENTA) : p->GetRenderObject()->SetColour(Debug::YELLOW);
	p->SetPhysicsObject(new PhysicsObject(&p->GetTransform(), p->GetBoundingVolume()));
	p->GetPhysicsObject()->InitSphereInertia(false);
	world->AddGameObject(p);
	return p;
}

bool TutorialGame::SelectObject() {
	if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
		RayCollision closestCollision;

		if (world->Raycast(ray, closestCollision, nullptr, true)) {
			if (selectionObject) {
				if (dynamic_cast<GameObjectEnemy*>(selectionObject))
					((GameObjectEnemy*)selectionObject)->SetDisplayDirection(false);
				if (dynamic_cast<EnemyBallBehaviourTree*>(selectionObject))
					((EnemyBallBehaviourTree*)selectionObject)->SetDisplayRoute(false);
				selectionObject == player ? selectionObject->GetRenderObject()->SetColour(Vector4(0, 0.5, 1, 1)) :
					selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject->SetSelected(false);
			}

			selectionObject = (GameObject*)closestCollision.node;
			if (dynamic_cast<GameObjectEnemy*>(selectionObject))
				((GameObjectEnemy*)selectionObject)->SetDisplayDirection(true);
			if (dynamic_cast<EnemyBallBehaviourTree*>(selectionObject))
				((EnemyBallBehaviourTree*)selectionObject)->SetDisplayRoute(true);
			selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
			selectionObject->SetSelected(true);
			return true;
		}
		return false;
	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
		if (selectionObject) {
			if (lockedObject == selectionObject) {
				camState = CameraState::FREE;
				lockedObject = nullptr;
			}
			else {
				camState = CameraState::THIRDPERSON;
				lockedObject = selectionObject;
			}
			world->GetMainCamera()->SetState(camState);
		}
	}
	return false;
}

void TutorialGame::DebugObjectMovement() {
	if (inSelectionMode && selectionObject) {
		selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT))
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT))
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP))
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 0, -10));
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN))
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 0, 10));
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM2))
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8))
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, nullptr, true)) {
				if (closestCollision.node == selectionObject)
					selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
}

void TutorialGame::LockedObjectMovement(float dt) {
	if (inSelectionMode && selectionObject)
		selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	Matrix4 view = world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();
	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); 
	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();
	Vector3 charForward = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);
	float force = 3.0f * dt;

	if (lockedObject == player) {
		renderer->DrawString("Jump Higher", Vector2(84, 25), Debug::WHITE, textSize);
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE))
			player->Jump();
		if (player->GetPowerUpTimer() > 0.0f) {
			force = 6.0f * dt;
			renderer->DrawString("Increase Speed:" + std::to_string((int)player->GetPowerUpTimer()), Vector2(40, 95), Debug::WHITE, textSize);
		}
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W))
		lockedObject->GetPhysicsObject()->ApplyLinearImpulse(fwdAxis * force);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A))
		lockedObject->GetPhysicsObject()->ApplyLinearImpulse(-rightAxis * force);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S))
		lockedObject->GetPhysicsObject()->ApplyLinearImpulse(-fwdAxis * force);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
		lockedObject->GetPhysicsObject()->ApplyLinearImpulse(rightAxis * force);
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::K))
		lockedOrientation = !lockedOrientation;
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::NUM1)) {
		switch (camState) {
		case CameraState::THIRDPERSON:
			camState = CameraState::TOPDOWN;
			break;
		case CameraState::TOPDOWN:
			camState = CameraState::THIRDPERSON;
			break;
		}
		world->GetMainCamera()->SetState(camState);
	}
}
