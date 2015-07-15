#ifndef GUI_H
#define GUI_H

#include "structArray.h"
#include "MyMath.h"
#include "SDL.h"


typedef int WidgetHandle;

struct GUI;
struct Widget;

struct GUI_WidgetID
{
	int upper;
	int lower;
	int widget;
};


struct GUI_WidgetType
{
	enum Type
	{
		CONTAINER,
		BUTTON,
		TEXTFIELD
	};
};


inline bool operator==(const GUI_WidgetID& w1, const GUI_WidgetID& w2) 
{
	return w1.upper == w2.upper && w1.lower == w2.lower && w1.widget == w2.widget;
}

inline bool operator!=(const GUI_WidgetID& w1, const GUI_WidgetID& w2)
{
	return !(w1 == w2);
}

static GUI_WidgetID GUI_MAIN_WIDGET = { 0, -1, 0 };
static GUI_WidgetID GUI_INVALID_WIDGET = { -1, -1, -1 };

typedef void(Callback)(GUI* gui, GUI_WidgetID id, void*);


struct GUI_ButtonHandle
{
	int WidgetID;
	int ButtonID;
};

struct GUI_TextfieldHandle
{
	int WidgetID;
	int TextfieldID;
};

struct GUI_Button
{
	const char* name;
	void* callbackArg;
	Callback* callback;
};

struct GUI_Textfield
{
	const char* text;
};

struct GUI_Container
{
	enum Orientation { VERTICAL, HORIZONTAL } orientation;
};

void GUI_initGUI(GUI** gui);
void GUI_shutdownGUI(GUI* gui);

void GUI_drawGUI(GUI* gui, SDL_Renderer* renderer);
void GUI_sendEventToGUI(GUI* gui, SDL_Event* event);




GUI_WidgetID GUI_addContainer(GUI* gui, GUI_WidgetID parentID, intRect rect, GUI_Container::Orientation orientation);
GUI_WidgetID GUI_addContainer(GUI* gui, GUI_WidgetID parentID, intRect rect, GUI_Container container);

GUI_WidgetID GUI_addButton(GUI* gui, GUI_WidgetID parentID, intRect rect, const char* name, void* args, Callback* callback);
GUI_WidgetID GUI_addButton(GUI* gui, GUI_WidgetID parentID, intRect rect, GUI_Button button);

GUI_WidgetID GUI_addTextfield(GUI* gui, GUI_WidgetID parentID, intRect rect, const char* text);
GUI_WidgetID GUI_addTextfield(GUI* gui, GUI_WidgetID parentID, intRect rect, GUI_Textfield textfield);

void GUI_removeWidget(GUI* gui, GUI_WidgetID widgetID);

GUI_WidgetID GUI_getParentWidget(GUI* gui, GUI_WidgetID widgetID);
int GUI_getNumChildren(GUI*gui, GUI_WidgetID widgetID);
GUI_WidgetID GUI_getChildWidget(GUI* gui, GUI_WidgetID widgetID, int childIndex);
GUI_WidgetType::Type GUI_getWidgetType(GUI* gui, GUI_WidgetID widgetID);

GUI_Button GUI_getButton(GUI* gui, GUI_WidgetID widgetID);
GUI_Textfield GUI_getTextfield(GUI* gui, GUI_WidgetID widgetID);

//void GUI_setButton(GUI* gui, GUI_WidgetID id, GUI_Button button);
//void GUI_setTextfield(GUI* gui, GUI_WidgetID id, GUI_Textfield textField);






#endif