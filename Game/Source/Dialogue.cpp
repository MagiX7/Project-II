#include "App.h"
#include "Render.h"
#include "DialogueManager.h"
#include "Dialogue.h"

DialogueOption::DialogueOption()
{
	id = 0; nextNodeId = 0;
}

DialogueOption::~DialogueOption()
{
}

Dialogue::Dialogue(int dialogueId) : id(dialogueId)
{
	isDialogueActive = false;
}

Dialogue::~Dialogue()
{
}

void Dialogue::Draw(int& count, Font* font)
{
	++count;
	if (count % 2 == 0 && currentNode->stopDialog == false)
	{
		if (currentNode->letterCount < currentNode->text.size())
		{
			textToPrint += currentNode->text.at(currentNode->letterCount);
			++currentNode->letterCount;
		}
		else currentNode->dialogFinished = true;
	}

	app->render->DrawText(font, textToPrint.c_str(), 0, 0, 100, 5, { 255,255,255 });

	// Render options for the player
	eastl::list<DialogueOption*>::iterator optionsIterator = currentNode->options.begin();
	if (nodes.size() > 1)
	{
		int offsetY = 0;
		for (; optionsIterator != (*currentNode).options.end(); ++optionsIterator)
		{
			app->render->DrawText(font, (*optionsIterator)->text.c_str(), 0, 110 + offsetY, 50, 5, { 0,255,0,255 });
			offsetY += 60;
		}
	}
}

bool Dialogue::CleanUp()
{

	return true;
}

void Dialogue::TriggerDialog(int id)
{
	isDialogueActive = true;
}

void Dialogue::DisableDialog(int id)
{
}

void NpcNode::Reset()
{
	this->dialogFinished = false;
	this->letterCount = 0;
	this->stopDialog = false;
}

DialogueOption* NpcNode::SetCurrentOptionById(int id)
{
	eastl::list<DialogueOption*>::iterator it = options.begin();
	int i = 0;
	while (it != options.end())
	{
		if (id == i)
		{
			currentOption = (*it);
			return (*it);
		}
		++i;
		++it;
	}
	return nullptr;
}
