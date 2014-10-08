#include "Main.h"

void CMain::OnExit()
{
	mainShader_vertex.release();
	mainShader_fragment.release();
	lightShader_fragment.release();
	mainProgram.release();
	skybox.release();

	SDL_DestroyWindow(Window_Main);
	Window_Main = NULL;

	IMG_Quit();
	SDL_Quit();
}