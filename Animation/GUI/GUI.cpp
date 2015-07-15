
#include "GUI.h"
#include "MyString.h"

#include <algorithm>
#include <vector>
#include "SDL_ttf.h"


TTF_Font* font;


//static GUI_WidgetID GUI_MAIN_WIDGET = { 0, -1 , 0};
//static GUI_WidgetID GUI_INVALID_WIDGET = { -1, -1, -1};


static const int BUTTON_MIN_SIZE_X = 50;
static const int BUTTON_MIN_SIZE_Y = 50;

static const int TEXTFIELD_MIN_SIZE_X = 50;
static const int TEXTFIELD_MIN_SIZE_Y = 50;



struct WidgetState
{
	enum State
	{
		NORMAL,
		HOVER,
		PRESSED,
	};
};

struct WidgetAlign
{
	enum Align
	{
		FIXED,
		FILL,
		MIN,
	};
};

struct Widget
{
	int type;
	int children;
	GUI_WidgetID widgetID;
	GUI_WidgetID parentID;
	int state;
	intRect originalRect;
};

struct TextWidget
{
	GUI_WidgetID widgetID;
	char* text;
	int cursorStart;
	int cursorEnd;
};

struct Button
{
	GUI_WidgetID id;
	char* name;
	void* callbackArg;
	Callback* callback;
};

struct Textfield
{
	GUI_WidgetID id;
	char* text;
};

struct Container
{
	GUI_WidgetID id;
	GUI_Container::Orientation orientation;
};


struct GUI
{
	GUI_WidgetID activeWidgetID;
	TextWidget activeTextWidget;

	std::vector<StructArray<intRect>> rects;
	std::vector<StructArray<Widget>> widgets;
	std::stack<int> freeUppers;
		
	StructArray<Button> buttons;
	StructArray<Textfield> textfields;
	StructArray<Container> containers;
};


void GUI_initGUI(GUI** gui)
{
	TTF_Init();
	font = TTF_OpenFont("arial.ttf", 14);
	
	*gui = new GUI;

	(*gui)->activeTextWidget.widgetID = GUI_INVALID_WIDGET;
	(*gui)->activeWidgetID = GUI_INVALID_WIDGET;

	(*gui)->rects.push_back(StructArray<intRect>());
	(*gui)->widgets.push_back(StructArray<Widget>());


}

void GUI_shutdownGUI(GUI* gui)
{
	delete gui;
}


void GUI_drawGUI(GUI* gui, SDL_Renderer* renderer)
{

	for (int i = 0; i < gui->buttons.size(); ++i)
	{
		Button& button = gui->buttons[i];
		if (button.id == GUI_INVALID_WIDGET)
			continue;


		Widget& widget = gui->widgets[button.id.upper][button.id.lower];
		intRect& rect = gui->rects[button.id.upper][button.id.lower];

		switch (widget.state)
		{
		case WidgetState::NORMAL:
			SDL_SetRenderDrawColor(renderer, 52, 73, 94, 255);
			break;

		case WidgetState::HOVER:
			SDL_SetRenderDrawColor(renderer, 41, 128, 185, 255);
			break;

		case WidgetState::PRESSED:
			SDL_SetRenderDrawColor(renderer, 52, 152, 219, 255);
			break;
		}

		
		SDL_RenderFillRect(renderer, (SDL_Rect*) &rect);
		
		SDL_Surface* surface = TTF_RenderText_Blended(font, button.name, { 255, 255, 255, 255 });

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

		SDL_Rect textPos;
		textPos.x = rect.x + rect.width/2 - surface->w / 2;
		textPos.y = rect.y + rect.height/2 - surface->h / 2;
		textPos.w = surface->w;
		textPos.h = surface->h;

		SDL_RenderCopy(renderer, texture, 0, &textPos);
		SDL_DestroyTexture(texture);
		SDL_FreeSurface(surface);
	}


	SDL_SetRenderDrawColor(renderer, 52, 73, 94, 255);
	for (int i = 0; i < gui->textfields.size(); ++i)
	{


		Textfield& textfield = gui->textfields[i];

		if (textfield.id == GUI_INVALID_WIDGET)
			continue;


		Widget& widget = gui->widgets[textfield.id.upper][textfield.id.lower];
		intRect& rect = gui->rects[textfield.id.upper][textfield.id.lower];

				
		SDL_RenderFillRect(renderer, (SDL_Rect*)&rect);

		SDL_Rect innerRect;
		innerRect.x = rect.x + 5;
		innerRect.y = rect.y + 5;
		innerRect.w = rect.width - 10;
		innerRect.h = rect.height - 10;

		SDL_SetRenderDrawColor(renderer, 44, 62, 80, 255);
		SDL_RenderFillRect(renderer, &innerRect);

		if (gui->activeTextWidget.widgetID == textfield.id)
		{

			TextWidget& textWidget = gui->activeTextWidget;

			if (textWidget.cursorStart != textWidget.cursorEnd)
			{
				SDL_SetRenderDrawColor(renderer, 52, 152, 219, 255);


				int start = textWidget.cursorStart < textWidget.cursorEnd ? textWidget.cursorStart : textWidget.cursorEnd;
				int end = textWidget.cursorStart > textWidget.cursorEnd ? textWidget.cursorStart : textWidget.cursorEnd;

				char firstSeg[128];
				char secondSeg[128];

				strncpy(firstSeg, textWidget.text, start);
				firstSeg[start] = 0;
				strncpy(secondSeg, textWidget.text + start, end - start);
				secondSeg[end - start] = 0;

				int firstW;
				int secondW;
				int h;

				TTF_SizeText(font, firstSeg, &firstW, &h);
				TTF_SizeText(font, secondSeg, &secondW, &h);

				SDL_Rect selectionBox;
				selectionBox.x = innerRect.x + firstW;
				selectionBox.y = innerRect.y;
				selectionBox.w = secondW;
				selectionBox.h = h;

				SDL_RenderFillRect(renderer, &selectionBox);
			}





			char cursorText[128];
			strncpy(cursorText, textWidget.text, textWidget.cursorStart);
			cursorText[textWidget.cursorStart] = 0;

			int w;
			int h;
			TTF_SizeText(font, cursorText, &w, &h);

			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

			SDL_RenderDrawLine(renderer, innerRect.x + MIN(w, innerRect.w), innerRect.y, innerRect.x + MIN(w, innerRect.w), innerRect.y + h);


			char text[256];
			strcpy(text, textWidget.text);

			int length = strlen(text);
			int width = 0;
			for (int j = 0; j < length; ++j)
			{
				char c[] = { text[j], 0 };
				int w;
				TTF_SizeText(font, c, &w, 0);
				width += w;

				if (width > innerRect.w)
				{
					char br[] = { '\n', 0 };
					insertString(text, j + 1, br);
					length++;
					width = 0;
				}

			}

		}

		SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, textfield.text, { 255, 255, 255, 255 }, 10000);
		

		if (surface)
		{

			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

			SDL_Rect dstRect;
			dstRect.x = innerRect.x;
			dstRect.y = innerRect.y;
			dstRect.w = MIN(innerRect.w, surface->w);
			dstRect.h = MIN(innerRect.h, surface->h);

			SDL_Rect srcRect;
			srcRect.x = 0;
			srcRect.y = 0;
			srcRect.w = MIN(innerRect.w, surface->w);
			srcRect.h = MIN(innerRect.h, surface->h);


			SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
			SDL_DestroyTexture(texture);
			SDL_FreeSurface(surface);
		}
	}


}

void GUI_sendEventToGUI(GUI* gui, SDL_Event* event)
{
	int2 mousePos = { 0, 0};
	SDL_GetMouseState(&mousePos.x, &mousePos.y);


	switch (event->type)
	{
	case SDL_MOUSEBUTTONDOWN:
	{

		if (gui->activeWidgetID == GUI_INVALID_WIDGET)
			break;

		Widget& activeWidget = gui->widgets[gui->activeWidgetID.upper][gui->activeWidgetID.lower];

		activeWidget = gui->widgets[gui->activeWidgetID.upper][gui->activeWidgetID.lower];
		activeWidget.state = WidgetState::PRESSED;

		switch (activeWidget.type)
		{
		case GUI_WidgetType::TEXTFIELD:
			gui->activeTextWidget.cursorStart = strlen(gui->textfields[gui->activeWidgetID.widget].text);
			gui->activeTextWidget.cursorEnd = gui->activeTextWidget.cursorStart;
			gui->activeTextWidget.widgetID = gui->activeWidgetID;
			gui->activeTextWidget.text = gui->textfields[gui->activeWidgetID.widget].text;

			SDL_StartTextInput();

			break;
		default:
			gui->activeTextWidget.widgetID = GUI_INVALID_WIDGET;
			
		}

		break;
	}
	case SDL_MOUSEBUTTONUP:
	{
		if (gui->activeWidgetID == GUI_INVALID_WIDGET)
			break;

		Widget& activeWidget = gui->widgets[gui->activeWidgetID.upper][gui->activeWidgetID.lower];

		switch (activeWidget.type)
		{
		case GUI_WidgetType::BUTTON:

			Button button = gui->buttons[gui->activeWidgetID.widget];
			if (activeWidget.state == WidgetState::PRESSED)
			{
				button.callback(gui, gui->activeWidgetID, button.callbackArg);
			}

			break;
		}

		activeWidget.state &= ~WidgetState::PRESSED;

		break;
	}
	case SDL_MOUSEMOTION:
	{
		int parent = 0;
		for (int i = 0; i < gui->rects[parent].size(); ++i)
		{
			if (pointInRect(mousePos, gui->rects[parent][i]))
			{
				Widget& widget = gui->widgets[parent][i];

				if (widget.children == -1)
				{
					if (widget.widgetID != gui->activeWidgetID)
					{
						if (gui->activeWidgetID != GUI_INVALID_WIDGET)
						{
							Widget& activeWidget = gui->widgets[gui->activeWidgetID.upper][gui->activeWidgetID.lower];
							activeWidget.state = WidgetState::NORMAL;
						}

						gui->activeWidgetID = widget.widgetID;

						widget.state = WidgetState::HOVER;
					}
					return;
				}

				parent = widget.children;
				i = -1;
				continue;
			}

		}

		if (gui->activeWidgetID != GUI_INVALID_WIDGET)
		{
			Widget& activeWidget = gui->widgets[gui->activeWidgetID.upper][gui->activeWidgetID.lower];
			activeWidget.state = WidgetState::NORMAL;

			gui->activeWidgetID = GUI_INVALID_WIDGET;
		}


		break;

	}
	case SDL_KEYDOWN:
	{

		TextWidget& textWidget = gui->activeTextWidget;

		if (textWidget.widgetID == GUI_INVALID_WIDGET)
			break;

		switch (event->key.keysym.sym)
		{
		case SDLK_RETURN:
		{


			int start = MIN(textWidget.cursorStart, textWidget.cursorEnd);
			int end = MAX(textWidget.cursorStart, textWidget.cursorEnd);

			eraseString(textWidget.text, start, end);

			textWidget.cursorStart = start;

			char br[] = { '\n', 0 };

			insertString(textWidget.text, textWidget.cursorStart, br);

			textWidget.cursorStart++;
			textWidget.cursorEnd = textWidget.cursorStart;

			break;
		}
		case SDLK_ESCAPE:
		{
			gui->widgets[textWidget.widgetID.upper][textWidget.widgetID.lower].state = WidgetState::NORMAL;
			textWidget.widgetID = GUI_INVALID_WIDGET;

			SDL_StopTextInput();
			break;
		}
		case SDLK_LEFT:
		{
			textWidget.cursorStart = MAX(textWidget.cursorStart - 1, 0);
			if (!(event->key.keysym.mod & KMOD_SHIFT))
			{
				textWidget.cursorEnd = textWidget.cursorStart;
			}
			break;
		}
		case SDLK_RIGHT:
		{
			textWidget.cursorStart = MIN(textWidget.cursorStart + 1, strlen(textWidget.text));

			if (!event->key.keysym.mod & KMOD_SHIFT)
			{
				textWidget.cursorEnd = textWidget.cursorStart;
			}
			break;
		}
		case SDLK_BACKSPACE:
		{
			if (textWidget.cursorStart != textWidget.cursorEnd)
			{
				int start = MIN(textWidget.cursorStart, textWidget.cursorEnd);
				int end = MAX(textWidget.cursorStart, textWidget.cursorEnd);

				eraseString(textWidget.text, start, end);

				textWidget.cursorStart = start;
			}
			else if (textWidget.cursorStart > 0)
			{
				eraseString(textWidget.text, textWidget.cursorStart - 1, textWidget.cursorStart);
				textWidget.cursorStart = MAX(textWidget.cursorStart - 1, 0);
			}
			textWidget.cursorEnd = textWidget.cursorStart;
			break;
		}

		case SDLK_DELETE:
		{
			if (textWidget.cursorStart != textWidget.cursorEnd)
			{
				int start = MIN(textWidget.cursorStart, textWidget.cursorEnd);
				int end = MAX(textWidget.cursorStart, textWidget.cursorEnd);

				eraseString(textWidget.text, start, end);

				textWidget.cursorStart = start;
			}
			else
			{
				eraseString(textWidget.text, textWidget.cursorStart, textWidget.cursorStart + 1);
			}
			textWidget.cursorEnd = textWidget.cursorStart;
			break;

		}
		}

		break;
	}
	case SDL_TEXTINPUT:
	{

		TextWidget& textWidget = gui->activeTextWidget;

		if (textWidget.widgetID == GUI_INVALID_WIDGET)
			break;

		int start = MIN(textWidget.cursorStart, textWidget.cursorEnd);
		int end = MAX(textWidget.cursorStart, textWidget.cursorEnd);

		eraseString(textWidget.text, start, end);

		textWidget.cursorStart = start;

		insertString(textWidget.text, textWidget.cursorStart, event->text.text);

		textWidget.cursorStart += strlen(event->text.text);
		textWidget.cursorEnd = textWidget.cursorStart;

		break;
	}


	}
}


GUI_WidgetID addWidget(GUI* gui, GUI_WidgetID parentID, intRect rect,  int type)
{

	if (parentID == GUI_MAIN_WIDGET)
	{
		GUI_WidgetID widgetID;

		widgetID.upper = 0;

		Widget widget;
		widget.state = WidgetState::NORMAL;
		widget.type = type;
		widget.children = -1;
		widget.originalRect = rect;
		widget.parentID = parentID;

		widgetID.lower = gui->widgets[widgetID.upper].push(widget);

		return widgetID;
	}


	Widget parentWidget = gui->widgets[parentID.upper][parentID.lower];

	if (parentWidget.type != GUI_WidgetType::CONTAINER)
		return GUI_INVALID_WIDGET;

	if (parentWidget.children == -1)
	{
		if (gui->freeUppers.size() > 0)
		{
			int id = gui->freeUppers.top();
			gui->widgets[parentID.upper][parentID.lower].children = id;

			//gui->rects[id] = StructArray<intRect>();
			//gui->widgets[id] = StructArray<Widget>();

		}
		else
		{
			gui->widgets[parentID.upper][parentID.lower].children = gui->widgets.size();

			gui->rects.push_back(StructArray<intRect>());
			gui->widgets.push_back(StructArray<Widget>());
		}
			
		parentWidget = gui->widgets[parentID.upper][parentID.lower];
	}
	
	GUI_WidgetID widgetID;

	widgetID.upper = parentWidget.children;
	
	Widget widget;
	widget.state = WidgetState::NORMAL;
	widget.type = type;
	widget.children = -1;
	widget.originalRect = rect;
	widget.parentID = parentID;
	
	widgetID.lower = gui->widgets[widgetID.upper].push(widget);
	
	return widgetID;
}


void addRect(GUI* gui, GUI_WidgetID parentID, intRect rect)
{
	if (parentID == GUI_MAIN_WIDGET)
	{
		// todo fill/min
		
		gui->rects[0].push(rect);
		return;
	}
	

	Widget parent = gui->widgets[parentID.upper][parentID.lower];
	intRect parentRect = gui->rects[parentID.upper][parentID.lower];
	
	gui->rects[parent.children].push(rect);

	
	switch (parent.type)
	{
	case GUI_WidgetType::CONTAINER:
	{
		Container container = gui->containers[parentID.widget];

		int fillSizeX = 0;
		int fillSizeY = 0;


		int fixedSize = 0;
		int numFills = 0;

		if (container.orientation = GUI_Container::HORIZONTAL)
		{
			for (int i = 0; i < gui->widgets[parent.children].size(); ++i)
			{
				intRect childRect = gui->widgets[parent.children][i].originalRect;

				if (childRect.width == -1)
					numFills++;
				else
					fixedSize += childRect.width;

			}

			fillSizeY = parentRect.height;
			fillSizeX = numFills != 0 ? (parentRect.width - fixedSize) / numFills : 0;
		
		}
		else if (container.orientation = GUI_Container::VERTICAL)
		{
			for (int i = 0; i < gui->widgets[parent.children].size(); ++i)
			{
				intRect childRect = gui->widgets[parent.children][i].originalRect;
				if (childRect.width == -1)
					numFills++;
				else
					fixedSize += childRect.width;
			}


			fillSizeX = parentRect.width;
			fillSizeY = numFills != 0 ? (parentRect.width - fixedSize) / numFills : 0;
		}

		int x = parentRect.x;
		int y = parentRect.y;
		for (int i = 0; i < gui->widgets[parent.children].size(); ++i)
		{
			intRect& originalRect = gui->widgets[parent.children][i].originalRect;
			intRect& realRect = gui->rects[parent.children][i];

			realRect.x = x;
			realRect.y = parentRect.y;

			if (originalRect.height == -1)
				realRect.height = fillSizeY;
			else if (originalRect.height == 0)
				realRect.height = 50;
			else
				realRect.height = originalRect.height;


			if (originalRect.width == -1)
				realRect.width = fillSizeX;
			else if (originalRect.width == 0)
				realRect.width = 50;
			else
				realRect.width = originalRect.width;

			if (container.orientation == GUI_Container::HORIZONTAL)
				x += realRect.width;
			else if (container.orientation == GUI_Container::VERTICAL)
				y += realRect.height;

		}

		break;
	}
	}
}



GUI_WidgetID GUI_addContainer(GUI* gui, GUI_WidgetID parentID, intRect rect, GUI_Container::Orientation orientation)
{
	GUI_WidgetID id = addWidget(gui, parentID, rect, GUI_WidgetType::CONTAINER);

	if (id == GUI_INVALID_WIDGET)
		return GUI_INVALID_WIDGET;

	addRect(gui, parentID, rect);

	Container container;
	container.orientation = orientation;

	id.widget = gui->containers.push(container);

	gui->widgets[id.upper][id.lower].widgetID = id;
	gui->containers[id.widget].id = id;
}

GUI_WidgetID GUI_addContainer(GUI* gui, GUI_WidgetID parentID, intRect rect, GUI_Container container)
{
	return GUI_addContainer(gui, parentID, rect, container.orientation);
}

GUI_WidgetID GUI_addButton(GUI* gui, GUI_WidgetID parentID, intRect rect, const char* name, void* args, Callback* callback)
{

	GUI_WidgetID id = addWidget(gui, parentID, rect, GUI_WidgetType::BUTTON);

	if (id == GUI_INVALID_WIDGET)
		return GUI_INVALID_WIDGET;

	addRect(gui, parentID, rect);

	Button button;
	button.callback = callback;
	button.callbackArg = args;
	button.name = new char[strlen(name)+1];
	strcpy(button.name, name);

	id.widget = gui->buttons.push(button);

	gui->widgets[id.upper][id.lower].widgetID = id;

	gui->buttons[id.widget].id = id;

}

GUI_WidgetID GUI_addButton(GUI* gui, GUI_WidgetID parentID, intRect rect, GUI_Button button)
{
	return GUI_addButton(gui, parentID, rect, button.name, button.callbackArg, button.callback);
}


GUI_WidgetID GUI_addTextfield(GUI* gui, GUI_WidgetID parentID, intRect rect, const char* text)
{
	GUI_WidgetID id = addWidget(gui, parentID, rect, GUI_WidgetType::TEXTFIELD);

	if (id == GUI_INVALID_WIDGET)
		return GUI_INVALID_WIDGET;

	addRect(gui, parentID, rect);

	Textfield textfield;
	textfield.text = new char[256];
	strcpy(textfield.text, text);

	id.widget = gui->textfields.push(textfield);

	gui->widgets[id.upper][id.lower].widgetID = id;
	gui->textfields[id.widget].id = id;
}

GUI_WidgetID GUI_addTextfield(GUI* gui, GUI_WidgetID parentID, intRect rect, GUI_Textfield textfield)
{
	return GUI_addTextfield(gui, parentID, rect, textfield.text);
}


void GUI_removeWidget(GUI* gui, GUI_WidgetID widgetID)
{
	Widget widget = gui->widgets[widgetID.upper][widgetID.lower];

	if (widgetID == gui->activeWidgetID)
	{
		gui->activeWidgetID = GUI_INVALID_WIDGET;
	}


	if (widget.children != -1)
	{
		for (int i = 0; i < gui->widgets[widget.children].size(); ++i)
		{
			GUI_removeWidget(gui, gui->widgets[widget.children][i].widgetID);
		}

		gui->widgets[widget.children].clear();
		gui->rects[widget.children].clear();

		gui->freeUppers.push(widget.children);

	}

	switch (widget.type)
	{
	case GUI_WidgetType::BUTTON:
		delete[] gui->buttons[widget.widgetID.widget].name;
		gui->buttons.free(widget.widgetID.widget, Button{ GUI_INVALID_WIDGET, 0, 0, 0 });
		break;
	case GUI_WidgetType::TEXTFIELD:
		delete[] gui->textfields[widget.widgetID.widget].text;

		if (gui->activeTextWidget.widgetID == widget.widgetID)
			gui->activeTextWidget.widgetID = GUI_INVALID_WIDGET;

		gui->textfields.free(widget.widgetID.widget, Textfield{ GUI_INVALID_WIDGET, 0, });
		break;
	case GUI_WidgetType::CONTAINER:
		gui->containers.free(widget.widgetID.widget, Container{ GUI_INVALID_WIDGET, (GUI_Container::Orientation)0 });
		break;
	}

	Widget freeWidget = { 0 };
	widget.widgetID = GUI_INVALID_WIDGET;

	gui->widgets[widgetID.upper].free(widgetID.lower, freeWidget);
	gui->rects[widgetID.upper].free(widgetID.lower, intRect{ 0 });


}

GUI_WidgetID GUI_getParentWidget(GUI* gui, GUI_WidgetID widgetID)
{
	return gui->widgets[widgetID.upper][widgetID.lower].parentID;
}

int GUI_getNumChildren(GUI*gui, GUI_WidgetID widgetID)
{
	Widget parent = gui->widgets[widgetID.upper][widgetID.lower];
	return gui->widgets[parent.children].size();
}

GUI_WidgetID GUI_getChildWidget(GUI* gui, GUI_WidgetID widgetID, int childIndex)
{
	Widget parent = gui->widgets[widgetID.upper][widgetID.lower];
	return gui->widgets[parent.children][childIndex].parentID;
}


GUI_WidgetType::Type GUI_getWidgetType(GUI* gui, GUI_WidgetID widgetID)
{
	return (GUI_WidgetType::Type)gui->widgets[widgetID.upper][widgetID.lower].type;
}

GUI_Button GUI_getButton(GUI* gui, GUI_WidgetID widgetID)
{
	GUI_Button button = { 0 };
	
	button.name = gui->buttons[widgetID.widget].name;
	button.callback = gui->buttons[widgetID.widget].callback;
	button.callbackArg = gui->buttons[widgetID.widget].callbackArg;

	return button;
}

GUI_Textfield GUI_getTextfield(GUI* gui, GUI_WidgetID widgetID)
{
	GUI_Textfield textfield;

	textfield.text = gui->textfields[widgetID.widget].text;
	
	return textfield;
}


//void GUI_setButton(GUI* gui, GUI_ButtonHandle handle, GUI_Button button)
//{
//	gui->buttonWidgets[handle.WidgetID].rect = button.rect;
//
//	strcpy(gui->buttons[handle.ButtonID].name, button.name);
//	gui->buttons[handle.ButtonID].callback = button.callback;
//	gui->buttons[handle.ButtonID].callbackArg = button.callbackArg;
//}
//
//void GUI_setTextfield(GUI* gui, GUI_TextfieldHandle handle, GUI_Textfield textfield)
//{
//	gui->textfieldWidgets[handle.WidgetID].rect = textfield.rect;
//
//	strcpy(gui->textfields[handle.TextfieldID].text, textfield.text);
//}
//
//
