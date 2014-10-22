//=============================================================================
#include "TextureBank_SDL.h"
#include "Main.h"
#include "FileManager.h"
#include "Log.h"

//=============================================================================
std::map<std::string, Texture*> TextureBank::TexList;

//=============================================================================
bool TextureBank::Init(const char* folder) {
	Cleanup();

	SDL_Renderer* Renderer = CMain::GetInstance()->GetRenderer();
	if(!Renderer) return false;

	std::vector<std::string> Files = FileManager::GetFilesInFolder(folder); // Relative to CWD

	for(auto Filename : Files) {
        std::string Ext = FileManager::GetFilenameExt(Filename);
		std::string ID  = FileManager::GetFilenameWithoutExt(Filename);

        // Skip all non-png files
        if(Ext != "png" && Ext != "jpg") continue;

		//Log("Add Texture : ID = %s : Filename = %s : Ext = %s", ID.c_str(), Filename.c_str(), Ext.c_str());
		AddTexture(Renderer, ID, Filename);
	}

	return true;
}

//-----------------------------------------------------------------------------
void TextureBank::Cleanup() {
	if(TexList.size() <= 0) return;

	for(auto& Iterator : TexList) {
		Texture* TheTexture = (Texture*)Iterator.second;

		if(TheTexture) {
			delete TheTexture;
			TheTexture = NULL;
		}
	}

	TexList.clear();
}

//=============================================================================
void TextureBank::AddTexture(SDL_Renderer* Renderer, std::string ID, std::string Filename) {
    if(ID == "") return;

    Texture* NewTexture = new Texture();

	assert(fopen(Filename.c_str(), "r") != NULL);

    if(NewTexture->Load(Renderer, Filename) == false) {
		Log("Unable to Load Texture: %s", ID.c_str());
		return;
	}

    TexList[ID] = NewTexture;
}

//-----------------------------------------------------------------------------
Texture* TextureBank::Get(std::string ID) {
	if(TexList.find(ID) == TexList.end()) return 0;

	return TexList[ID];
}

//=============================================================================
