#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/ObjectStateGame.h"
#include "../CSC8503Common/NavigationGrid.h"
#include "../CSC8503Common/EvilBallEnemy.h"
#include "../CSC8503Common/EvilBall.h"
#include "../CSC8503Common/MovementFloor.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include "../CSC8503Common/PositionConstraint.h"
#include "../CSC8503Common/CrossObject.h"
#include "../CSC8503Common/GroundObject.h"
#include "../CSC8503Common/TileGround.h"
#include "../CSC8503Common/CollisionCrash.h"
#include "../CSC8503Common/CubeObject.h"
#include "../CSC8503Common/SphereObject.h"
#include "../CSC8503Common/SpinningTile.h"
#include "../CSC8503Common/ThisSpring.h"
#include "../CSC8503Common/PlayerBall.h"
#include "../CSC8503Common/EnemyObject.h"
#include "../CSC8503Common/CoinObject.h"
#include "../CSC8503Common/PowerupObject.h"
#include "../CSC8503Common/CapsuleObject.h"
#include "../CSC8503Common/GameEnd.h"
#include "../CSC8503Common/Pushdownstate.h"
#include "../CSC8503Common/PauseScreen.h"
#include "../CSC8503Common/GlobalVariables.h"
#include "../CSC8503Common/EnemyBallBehaviourTree.h"

namespace NCL {
	namespace CSC8503 {
		class PlayerBall;
		enum class FinishType { INGAME, TIMEOUT, WIN, LOSE };
		class TutorialGame : public PushdownState {
		public:
			TutorialGame();
			~TutorialGame();

			PushdownResult OnUpdate(float dt, PushdownState** newState) override {
				fpsTimer -= dt;
				++framesPerSecond;
				if (fpsTimer < 0.0f) {
					float alpha = 0.1f;
					avgFps = alpha * avgFps + (1.0 - alpha) * framesPerSecond;
					framesPerSecond = 0;
					fpsTimer = 1.0f;
				}

				currentLevel == 0 ? UpdateMenu(dt) : UpdateLevel(dt);
				physics->ClearDeletedCollisions();
				Debug::FlushRenderables(dt);
				world->UpdateWorld(dt);
				renderer->Update(dt);
				renderer->Render();

				
				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P)) {
					if (currentLevel != 0) {
						*newState = new PauseScreen();
						renderer->DrawString("Game Paused", Vector2(20, 30), Debug::WHITE, 30.0f);
						renderer->Render();
						return PushdownResult::Push;
					}
				}

				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE) || timeOut > 5.0f) {
					if (currentLevel != 0) {
						renderer->DrawString("Back to Menu", Vector2(15, 30), Debug::WHITE, 30.0f);
						renderer->Render();
					}
					else
						quit = true;
					return PushdownResult::Pop;
				}
				return PushdownResult::NoChange;
			};
			void OnAwake() override {
			}
			void UpdateMenu(float dt);
			void UpdateLevel(float dt);
			void DrawDebugInfo();
			void CheckFinished(float dt);
			void UpdateLevel1(float dt);
			void FireObjects();
			void UpdateLevel2(float dt);
			void EnemyRaycast();

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();
			void InitFloors(int level);
			void CreateMaze();
			void InitGameExamples(int level);
			void InitPlayer();
			void InitGameObstacles(int level);
			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			
			GameObject* AddFloorToWorld(GameObject* floor, const Vector3& position, const Vector3& size, const Matrix4& orientation = Matrix4());
			GameObject* AddSphereToWorld(GameObject* sphere, const Vector3& position, float radius);
			GameObject* AddCubeToWorld(GameObject* cube, const Vector3& position, Vector3 dimensions);
			GameObject* AddCapsuleToWorld(GameObject* capsule, const Vector3& position, float halfHeight, float radius);
			void AddBridgeToWorld(Vector3 startPos);

			GameObject* AddPlayerToWorld(GameObject* p, const Vector3& position);
			GameObject* AddEnemyToWorld(GameObject* e, const Vector3& position);
			GameObject* AddPickupToWorld(GameObject* e, const Vector3& position);

			bool SelectObject();
			void DebugObjectMovement();
			void LockedObjectMovement(float dt);

			GameTechRenderer* renderer;
			PhysicsSystem* physics;
			GameWorld* world;

			bool useGravity;
			bool useBroadphase;
			bool inSelectionMode;
			float forceMagnitude;
			float reloadTime;
			float timeOut;
			GameObject* selectionObject = nullptr;

			OGLMesh*	capsuleMesh = nullptr;
			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* baseTexture	= nullptr;
			OGLTexture* groundTileTexture = nullptr;
			OGLTexture* portalTexture = nullptr;
			OGLTexture* crossTexture = nullptr;
			OGLTexture* jumpingtileTexture = nullptr;
			OGLTexture* projectionTexture = nullptr;
			OGLTexture* woodTexture = nullptr;
			OGLTexture* endTexture = nullptr;
			OGLTexture* mainmenuTexture = nullptr;
			OGLTexture* planeTexture = nullptr;
			OGLTexture* redbrickTexture = nullptr;
			OGLShader*	gametechShader = nullptr;

			OGLMesh*	charMeshA	= nullptr;
			OGLMesh*	charMeshB	= nullptr;
			OGLMesh*	enemyMesh	= nullptr;
			OGLMesh*	bonusMesh	= nullptr;
	
			GameObject* lockedObject	= nullptr;
		
			PlayerBall* player;
			vector<GameObjectEnemy*> enemies;
			vector<MovementFloor*> platforms;
			int currentLevel;
			bool lockedOrientation;

			float textSize = 15.0f;
			int currentlySelected;
			vector<GameObject*> menuEnemies;
			vector<GameObject*> menuPlayers;

			int avgFps;
			int framesPerSecond;
			float fpsTimer;

			CameraState camState;
			vector<GameObject*> projectiles;
			FinishType finish;

			vector<EnemyBallBehaviourTree*> behaviourEnemies;

			GameObject* currentEnemy = nullptr;
		};
	}
}

