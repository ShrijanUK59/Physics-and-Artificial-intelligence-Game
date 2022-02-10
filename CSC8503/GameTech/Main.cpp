#include "TutorialGame.h"
#include "../../Common/Window.h"
#include "../CSC8503Common/PushdownMachine.h"
#include "../CSC8503Common/ScreenStart.h"
using namespace NCL;
using namespace CSC8503;
void GamePushdownAutomata(Window* w);

int main() {
	Window* w = Window::CreateGameWindow("CSC8503 Shrijan Tiwari Assignment", 1920, 1080, true);
	if (!w->HasInitialised())
		return -1;

	srand(time(0));
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);
	w->GetTimer()->GetTimeDeltaSeconds(); 

	GamePushdownAutomata(w);
	Window::DestroyGameWindow();		
}


void GamePushdownAutomata(Window* w) {
	PushdownMachine machine(new ScreenStart);
	while (w->UpdateWindow()) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR))
			w->ShowConsole(true);
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT))
			w->ShowConsole(false);
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T))
			w->SetWindowPosition(0, 0);
		if (!machine.Update(dt))
			return;
	}
}