#ifndef __GUIMANAGER_H__
#define __GUIMANAGER_H__

#include "Module.h"
#include "GuiControl.h"

#include "List.h"

class GuiManager : public Module
{
public:

	GuiManager();

	// Destructor
	virtual ~GuiManager();

	bool Update(float dt);

	bool Draw();

	// Called before quitting
	bool CleanUp();

	// Additional methods
	GuiControl* CreateGuiControl(GuiControlType type, uint32 id, SDL_Rect bounds, const char* name, SceneMenu* menu);
	void DestroyGuiControl(GuiControl* entity);

public:

	List<GuiControl*> controls;

	float accumulatedTime = 0.0f;
	float updateMsCycle = 0.0f;
	bool doLogic = false;
};

#endif // __GUIMANAGER_H__
