#ifndef _MENU_TREE_H_
#define _MENU_TREE_H_

#include <forward_list>
#include "Common.h"
#include "Font_SDL.h"

class MenuNode
{
public:
	MenuNode()
	{
		
	}

	MenuNode(std::string name, Font_SDL* font, SDL_Renderer* renderer, SDL_Color color, MenuNode* parent = NULL) 
	{
		this->Name = name; 
		this->Parent = parent;
		text.Load(font->GetFont(), name, renderer, color);
		font->SetOutline(1);
		hover.Load(font->GetFont(), name, renderer, color);
		font->SetOutline(0);
	}

	void AddCallBack(void(*function)())
	{
		callback = function;
	}

	void AddSubMenu(MenuNode* node)
	{
		if (node == NULL)	return;
		SubMenu.insert_after(SubMenu.end(), node);
	}

	std::string GetName()
	{
		return Name;
	}

	MenuNode* GetParent()
	{
		return Parent;
	}

	std::forward_list<MenuNode*> * GetChildren()
	{
		return &SubMenu;
	}

	Text_SDL* GetText()
	{
		return &text;
	}

	Text_SDL* GetTextHover()
	{
		return &hover;
	}

	void OnExecute()
	{
		callback();
	}

private:
	std::string Name;
	MenuNode* Parent;
	std::forward_list<MenuNode*> SubMenu;

	//Font texture of the node
	Text_SDL text;
	Text_SDL hover;

	void(*callback)();
};

#endif