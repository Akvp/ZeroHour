#include "LoadingScreen.h"
#include "Main.h"

SDL_Thread* CLoadingScreen::Thread_Loading = NULL;
int* CLoadingScreen::Condition = NULL;
int CLoadingScreen::StartTime = 0;

void CLoadingScreen::OnActivate(int* condition)
{
	Condition = condition;
	//Create a new thread
	Thread_Loading = SDL_CreateThread(OnExecute, "LoadingThread", NULL);
}

SDL_Thread* CLoadingScreen::GetThreadID()
{
	return Thread_Loading;
}

int CLoadingScreen::OnExecute(void* ptr)
{
	StartTime = SDL_GetTicks();
	OnLoad();
	while (*Condition == 0)
	{
		OnUpdate();
		OnRender();
		SDL_Delay(10);
	}
	CSpark::Clear();
	return 0;
}

void CLoadingScreen::OnLoad()
{
	//Random seed
	srand((unsigned int)(time(0)));
	//Load sprite textures
	TextureBank::Init("gfx/img/sprites");
	CSpark::Load(10);
	//Add 10 random sparks
	for (int i = 0; i < 10; i++)
	{
		CSpark::AddRandomSpark();
	}
}

void CLoadingScreen::OnUpdate()
{
	if (RandNormal() > 0.5)
	{
		CSpark::AddRandomSpark();
	}
//	if (SDL_GetTicks() - StartTime >= 3000)
//		*Condition = -1;
}

void CLoadingScreen::OnRender()
{
	CSpark::OnRender();
}

void CLoadingScreen::OnUnload()
{
	CSpark::Clear();
}

void CLoadingScreen::OnExit()
{
	TextureBank::Cleanup();
}

//=============================================================================

int CSpark::freeID = 0;
int CSpark::count = 0;
std::vector<CSpark> CSpark::sparkList;
std::vector<Texture_SDL*> CSpark::textureList;

CSpark::CSpark(SPARK_TYPE type, int size, int x, int y)
{
	this->frame = 0;
	this->type = type;
	this->size = size;
	this->ID = freeID++;
	this->xpos = x; this->ypos = y;
	switch (type)
	{
	case BLUE_SPARK:
		maxFrames = 83;
		spriteFile = "blue_spark";
		tileSize = 100;
		framePerRow = 5;
		break;
	case ORANGE_SPARK:
		maxFrames = 39;
		spriteFile = "orange_spark";
		tileSize = 141;
		framePerRow = 5;
		break;
	case PINK_SPARK:
		maxFrames = 40;
		spriteFile = "pink_spark";
		tileSize = 133;
		framePerRow = 5;
		break;
	}
	this->count++;
}

void CSpark::Load(int size)
{
	std::string files[] = { "blue_spark.png", "orange_spark.jpg", "pink_spark.jpg" };
	for (int i = 0; i < 3; i++)
	{
		textureList.push_back(new Texture_SDL());
		textureList[i]->Load(CMain::GetInstance()->GetRenderer(), "gfx/img/sprites/" + files[i], 0, 0, 0);
	}
	sparkList.reserve(size);
}

void CSpark::AddRandomSpark()
{
	int type = RandInt(0, 2);
	int size = RandInt(20, 80);
	int x = RandInt(0, CParams::WindowWidth - size);
	int y = RandInt(0, CParams::WindowHeight - size);
	AddSpark((SPARK_TYPE)type, size, x, y);
}

void CSpark::AddSpark(SPARK_TYPE type, int size, int x, int y)
{
	sparkList.push_back(CSpark(type, size, x, y));
}

void CSpark::Clear()
{
	sparkList.clear(); 
	sparkList.shrink_to_fit();
	for (int i = 0; i < 3; i++)
	{
		delete textureList[i];
	}
}

void CSpark::KillSpark(int ID)
{
	int index;
	for (int i = 0; i < sparkList.size(); i++)
	{
		if (ID == sparkList[i].ID)
		{
			index = i;
			break;
		}
	}
	sparkList.erase(sparkList.begin() + index, sparkList.begin() + index + 1);
	count--;
}

void CSpark::OnRender()
{
	SDL_RenderClear(CMain::GetInstance()->GetRenderer());
	for (int i = 0; i < sparkList.size(); i++)
	{
		int x, y;
		x = (sparkList[i].frame % sparkList[i].framePerRow) * sparkList[i].tileSize;
		y = (sparkList[i].frame / sparkList[i].framePerRow) * sparkList[i].tileSize;
		textureList[sparkList[i].type]->Render(sparkList[i].xpos, sparkList[i].ypos, sparkList[i].size, sparkList[i].size, x, y, sparkList[i].tileSize, sparkList[i].tileSize);
		sparkList[i].frame++;
		if (sparkList[i].frame == sparkList[i].maxFrames) KillSpark(sparkList[i].ID);
	}
	SDL_RenderPresent(CMain::GetInstance()->GetRenderer());
}