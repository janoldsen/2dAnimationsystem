
#include "GUI.h"
#include "MyString.h"

#include <algorithm>

#include "SDL_ttf.h"


TTF_Font* font;


struct Widget
{
	intRect rect;
};

struct Button
{
	char name[32];
	void* callbackArg;
	Callback* callback;
};


struct Textfield
{
	char text[128];
};

struct ActiveTextfield
{
	int id;
	int cursorStart;
	int cursorEnd;
};


struct GUI
{
	StructArray<Widget> buttonWidgets;
	StructArray<Button> buttons;
	StructArray<int> inactiveButtons;
	StructArray<int> hoveredButtons;
	StructArray<int> pressedButtons;


	StructArray<Widget> textfieldWidgets;
	StructArray<Textfield> textfields;
	StructArray<int> inactiveTextfields;
	StructArray<ActiveTextfield> activeTextfields;
};


void GUI_initGUI(GUI** gui)
{
	TTF_Init();
	font = TTF_OpenFont("arial.ttf", 14);
	
	*gui = new GUI;
}

void GUI_drawGUI(GUI* gui, SDL_Renderer* renderer)
{

	auto cmp = [](const void* a, const void* b)->int{return *(int*)a < *(int*)b; };

	qsort(&gui->inactiveButtons[0], gui->inactiveButtons.size(), sizeof(int), cmp);
	qsort(&gui->hoveredButtons[0], gui->hoveredButtons.size(), sizeof(int), cmp);
	qsort(&gui->pressedButtons[0], gui->pressedButtons.size(), sizeof(int), cmp);
	qsort(&gui->inactiveTextfields[0], gui->inactiveTextfields.size(), sizeof(int), cmp);
	qsort(&gui->activeTextfields[0], gui->activeTextfields.size(), sizeof(ActiveTextfield),
		[](const void* a, const void* b)->int{return ((ActiveTextfield*)a)->id < ((ActiveTextfield*)b)->id; });



	SDL_SetRenderDrawColor(renderer, 52, 73, 94, 255);
	for (int i = 0; i < gui->inactiveButtons.size(); ++i)
	{
		int id = gui->inactiveButtons[i];
		SDL_RenderFillRect(renderer, (SDL_Rect*) &gui->buttonWidgets[id].rect);
		
		SDL_Surface* surface = TTF_RenderText_Blended(font, gui->buttons[id].name, { 255, 255, 255, 255 });

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

		SDL_Rect textPos;
		textPos.x = gui->buttonWidgets[id].rect.x + gui->buttonWidgets[id].rect.width/2 - surface->w / 2;
		textPos.y = gui->buttonWidgets[id].rect.y + gui->buttonWidgets[id].rect.height/2 - surface->h / 2;
		textPos.w = surface->w;
		textPos.h = surface->h;

		SDL_RenderCopy(renderer, texture, 0, &textPos);
		SDL_FreeSurface(surface);
	}





	SDL_SetRenderDrawColor(renderer, 41, 128, 185, 255);
	for (int i = 0; i < gui->hoveredButtons.size(); ++i)
	{
		int id = gui->hoveredButtons[i];
		SDL_RenderFillRect(renderer, (SDL_Rect*)&gui->buttonWidgets[id].rect);

		SDL_Surface* surface = TTF_RenderText_Blended(font, gui->buttons[id].name, { 255, 255, 255, 255 });

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

		SDL_Rect textPos;
		textPos.x = gui->buttonWidgets[id].rect.x + gui->buttonWidgets[id].rect.width / 2 - surface->w / 2;
		textPos.y = gui->buttonWidgets[id].rect.y + gui->buttonWidgets[id].rect.height / 2 - surface->h / 2;
		textPos.w = surface->w;
		textPos.h = surface->h;

		SDL_RenderCopy(renderer, texture, 0, &textPos);
		SDL_FreeSurface(surface);
	}

	SDL_SetRenderDrawColor(renderer, 52, 152, 219, 255);
	for (int i = 0; i < gui->pressedButtons.size(); ++i)
	{
		int id = gui->pressedButtons[i];
		SDL_RenderFillRect(renderer, (SDL_Rect*)&gui->buttonWidgets[id].rect);
		SDL_Surface* surface = TTF_RenderText_Blended(font, gui->buttons[id].name, { 255, 255, 255, 255 });

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

		SDL_Rect textPos;
		textPos.x = gui->buttonWidgets[id].rect.x + gui->buttonWidgets[id].rect.width / 2 - surface->w / 2;
		textPos.y = gui->buttonWidgets[id].rect.y + gui->buttonWidgets[id].rect.height / 2 - surface->h / 2;
		textPos.w = surface->w;
		textPos.h = surface->h;

		SDL_RenderCopy(renderer, texture, 0, &textPos);
		SDL_FreeSurface(surface);
	}


	for (int i = 0; i < gui->inactiveTextfields.size(); ++i)
	{
		SDL_SetRenderDrawColor(renderer, 52, 73, 94, 255);


		int id = gui->inactiveTextfields[i];
		SDL_RenderFillRect(renderer, (SDL_Rect*)&gui->textfieldWidgets[id].rect);



		SDL_Rect innerRect;
		innerRect.x = gui->textfieldWidgets[id].rect.x + 5;
		innerRect.y = gui->textfieldWidgets[id].rect.y + 5;
		innerRect.w = gui->textfieldWidgets[id].rect.width - 10;
		innerRect.h = gui->textfieldWidgets[id].rect.height - 10;

		SDL_SetRenderDrawColor(renderer, 44, 62, 80, 255);
		SDL_RenderFillRect(renderer, &innerRect);


		char text[256];
		strcpy(text, gui->textfields[id].text);

		int length = strlen(text);
		int width = 0;
		for (int j = 0; j < length; ++j)
		{
			char c[] = { text[j], 0 };
			int w, h;
			TTF_SizeText(font, c, &w, &h);
			width += w;

			if (width > innerRect.w)
			{
				char br[] = { '\n', 0 };
				insertString(text, j, br);
				length++;
				width = 0;
			}

		}

		
		SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text, { 255, 255, 255, 255 }, innerRect.w);
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
			SDL_FreeSurface(surface);
		}
	}


	for (int i = 0; i < gui->activeTextfields.size(); ++i)
	{
		SDL_SetRenderDrawColor(renderer, 52, 73, 94, 255);

		ActiveTextfield activeTextfield = gui->activeTextfields[i];
		int id = activeTextfield.id;
		SDL_RenderFillRect(renderer, (SDL_Rect*)&gui->textfieldWidgets[id].rect);



		SDL_Rect innerRect;
		innerRect.x = gui->textfieldWidgets[id].rect.x + 5;
		innerRect.y = gui->textfieldWidgets[id].rect.y + 5;
		innerRect.w = gui->textfieldWidgets[id].rect.width - 10;
		innerRect.h = gui->textfieldWidgets[id].rect.height - 10;

		SDL_SetRenderDrawColor(renderer, 44, 62, 80, 255);
		SDL_RenderFillRect(renderer, &innerRect);


		if (gui->activeTextfields[i].cursorStart != gui->activeTextfields[i].cursorEnd)
		{
			SDL_SetRenderDrawColor(renderer, 52, 152, 219, 255);


			int start = activeTextfield.cursorStart < activeTextfield.cursorEnd ? activeTextfield.cursorStart : activeTextfield.cursorEnd;
			int end = activeTextfield.cursorStart > activeTextfield.cursorEnd ? activeTextfield.cursorStart : activeTextfield.cursorEnd;

			char firstSeg[128];
			char secondSeg[128];
			
			strncpy(firstSeg, gui->textfields[id].text, start);
			firstSeg[start] = 0;
			strncpy(secondSeg, gui->textfields[id].text+start, end-start);
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
		strncpy(cursorText, gui->textfields[id].text, activeTextfield.cursorStart);
		cursorText[activeTextfield.cursorStart] = 0;

		int w;
		int h;
		TTF_SizeText(font, cursorText, &w, &h);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		
		SDL_RenderDrawLine(renderer, innerRect.x + MIN(w, innerRect.w), innerRect.y, innerRect.x + MIN(w, innerRect.w), innerRect.y + h);


		char text[256];
		strcpy(text, gui->textfields[id].text);
		
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
				insertString(text, j+1, br);
				length++;
				width = 0;
			}

		}



		SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text, { 255, 255, 255, 255 }, 10000);
		
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
			SDL_FreeSurface(surface);
		}
	}


}

void GUI_sendEventToGUI(GUI* gui, SDL_Event* event)
{
	int2 mousePos = { event->motion.x, event->motion.y };
	switch (event->type)
	{
	case SDL_MOUSEBUTTONDOWN:
	{

		for (int i = 0; i < gui->buttonWidgets.size(); i++)
		{
			if (pointInRect(mousePos, gui->buttonWidgets[i].rect))
			{
				gui->buttons[i].callback(gui->buttons[i].callbackArg);
				gui->inactiveButtons.removeStr(i);
				gui->pressedButtons.push(i);
			}
		}


		for (int i = 0; i < gui->activeTextfields.size(); ++i)
		{
			gui->inactiveTextfields.push(gui->activeTextfields[i].id);
		}
		gui->activeTextfields.clear();

		for (int i = 0; i < gui->textfieldWidgets.size(); i++)
		{
			if (pointInRect(mousePos, gui->textfieldWidgets[i].rect))
			{
				ActiveTextfield activeTextField;
				activeTextField.cursorStart = strlen(gui->textfields[i].text);
				activeTextField.cursorEnd = activeTextField.cursorStart;
				activeTextField.id = i;

				gui->inactiveTextfields.removeStr(i);
				gui->activeTextfields.push(activeTextField);

				SDL_StartTextInput();
				SDL_SetTextInputRect((SDL_Rect*)&gui->textfieldWidgets[i].rect);
			}
		}
		break;
	}
	case SDL_MOUSEBUTTONUP:
	{
		for (int i = 0; i < gui->pressedButtons.size(); i++)
		{
			int id = gui->pressedButtons[i];
			if (pointInRect(mousePos, gui->buttonWidgets[id].rect))
			{
				gui->hoveredButtons.push(id);
			}
			else
			{
				gui->inactiveButtons.push(id);
			}
		}
		gui->pressedButtons.clear();

		break;
	}
	case SDL_MOUSEMOTION:
	{
		for (int i = 0; i < gui->hoveredButtons.size(); i++)
		{
			int id = gui->hoveredButtons[i];
			if (!pointInRect(mousePos, gui->buttonWidgets[id].rect))
			{
				gui->hoveredButtons.removeID(i);
				gui->inactiveButtons.push(id);

				i--;
			}

		}

		for (int i = 0; i < gui->inactiveButtons.size(); i++)
		{
			int id = gui->inactiveButtons[i];
			if (pointInRect(mousePos, gui->buttonWidgets[id].rect))
			{
				gui->hoveredButtons.push(id);
				gui->inactiveButtons.removeID(i);

				i--;
			}

		}
		break;
	}
	case SDL_KEYDOWN:
	{
		switch (event->key.keysym.sym)
		{
		case SDLK_RETURN:
		{
			for (int i = 0; i < gui->buttonWidgets.size(); i++)
			{
				if (pointInRect(mousePos, gui->buttonWidgets[i].rect))
				{
					gui->buttons[i].callback(gui->buttons[i].callbackArg);
					gui->inactiveButtons.removeStr(i);
					gui->pressedButtons.push(i);
				}
			}


			for (int i = 0; i < gui->activeTextfields.size(); i++)
			{
				int id = gui->activeTextfields[i].id;

				int start = MIN(gui->activeTextfields[i].cursorStart, gui->activeTextfields[i].cursorEnd);
				int end = MAX(gui->activeTextfields[i].cursorStart, gui->activeTextfields[i].cursorEnd);

				eraseString(gui->textfields[id].text, start, end);

				gui->activeTextfields[i].cursorStart = start;

				char br[] = { '\n', 0 };

				insertString(gui->textfields[id].text, gui->activeTextfields[i].cursorStart, br);

				gui->activeTextfields[i].cursorStart++;
				gui->activeTextfields[i].cursorEnd = gui->activeTextfields[i].cursorStart;
			}
			break;

			break;
		}
		case SDLK_ESCAPE:
		{
			for (int i = 0; i < gui->activeTextfields.size(); ++i)
			{
				gui->inactiveTextfields.push(gui->activeTextfields[i].id);
			}
			gui->activeTextfields.clear();
			SDL_StopTextInput();

			break;
		}
		case SDLK_LEFT:
		{
			for (int i = 0; i < gui->activeTextfields.size(); ++i)
			{
				gui->activeTextfields[i].cursorStart = MAX(gui->activeTextfields[i].cursorStart-1, 0);

				if (!(event->key.keysym.mod & KMOD_SHIFT))
				{
					gui->activeTextfields[i].cursorEnd = gui->activeTextfields[i].cursorStart;
				}

			}
			break;
		}
		case SDLK_RIGHT:
		{
			for (int i = 0; i < gui->activeTextfields.size(); ++i)
			{
				gui->activeTextfields[i].cursorStart = MIN(gui->activeTextfields[i].cursorStart+1, strlen(gui->textfields[gui->activeTextfields[i].id].text));

				if (!event->key.keysym.mod & KMOD_SHIFT)
				{
					gui->activeTextfields[i].cursorEnd = gui->activeTextfields[i].cursorStart;
				}

			}
			break;
		}
		case SDLK_BACKSPACE:
		{
			for (int i = 0; i < gui->activeTextfields.size(); ++i)
			{

				int id = gui->activeTextfields[i].id;

				if (gui->activeTextfields[i].cursorStart != gui->activeTextfields[i].cursorEnd)
				{
					int start = MIN(gui->activeTextfields[i].cursorStart, gui->activeTextfields[i].cursorEnd);
					int end = MAX(gui->activeTextfields[i].cursorStart, gui->activeTextfields[i].cursorEnd);

					eraseString(gui->textfields[id].text, start, end);

					gui->activeTextfields[i].cursorStart = start;
				}
				else if (gui->activeTextfields[i].cursorStart > 0)
				{
					eraseString(gui->textfields[id].text, gui->activeTextfields[i].cursorStart - 1, gui->activeTextfields[i].cursorStart);
					gui->activeTextfields[i].cursorStart = MAX(gui->activeTextfields[i].cursorStart-1, 0);
				}
				gui->activeTextfields[i].cursorEnd = gui->activeTextfields[i].cursorStart;
			}
			break;
		}
		case SDLK_DELETE:
		{
			for (int i = 0; i < gui->activeTextfields.size(); ++i)
			{

				int id = gui->activeTextfields[i].id;

				if (gui->activeTextfields[i].cursorStart != gui->activeTextfields[i].cursorEnd)
				{
					int start = MIN(gui->activeTextfields[i].cursorStart, gui->activeTextfields[i].cursorEnd);
					int end = MAX(gui->activeTextfields[i].cursorStart, gui->activeTextfields[i].cursorEnd);

					eraseString(gui->textfields[id].text, start, end);

					gui->activeTextfields[i].cursorStart = start;
				}
				else
				{
					eraseString(gui->textfields[id].text, gui->activeTextfields[i].cursorStart, gui->activeTextfields[i].cursorStart+1);
				}
				gui->activeTextfields[i].cursorEnd = gui->activeTextfields[i].cursorStart;
			}
			break;
		}
		}
		break;
	}
	case SDL_TEXTINPUT:
	{

		for (int i = 0; i < gui->activeTextfields.size(); i++)
		{
			int id = gui->activeTextfields[i].id;

			int start = MIN(gui->activeTextfields[i].cursorStart, gui->activeTextfields[i].cursorEnd);
			int end = MAX(gui->activeTextfields[i].cursorStart, gui->activeTextfields[i].cursorEnd);

			eraseString(gui->textfields[id].text, start, end);

			gui->activeTextfields[i].cursorStart = start;

			insertString(gui->textfields[id].text, gui->activeTextfields[i].cursorStart, event->text.text);

			gui->activeTextfields[i].cursorStart += strlen(event->text.text);
			gui->activeTextfields[i].cursorEnd = gui->activeTextfields[i].cursorStart;
		}
		break;
	}
	
	}

}

GUI_ButtonHandle GUI_addButton(GUI* gui, GUI_Button button)
{
	GUI_ButtonHandle handle;

	handle.WidgetID = gui->buttonWidgets.push(Widget());
	handle.ButtonID = gui->buttons.push(Button());

	gui->inactiveButtons.push(handle.ButtonID);

	GUI_setButton(gui, handle, button);

	return handle;
}


GUI_TextfieldHandle GUI_addTextfield(GUI* gui, GUI_Textfield textfield)
{
	GUI_TextfieldHandle handle;

	handle.WidgetID = gui->textfieldWidgets.push(Widget());
	handle.TextfieldID = gui->textfields.push(Textfield());

	gui->inactiveTextfields.push(handle.TextfieldID);

	GUI_setTextfield(gui, handle, textfield);

	return handle;
}


GUI_Button GUI_getButton(GUI* gui, GUI_ButtonHandle handle)
{
	GUI_Button button = { 0 };

	button.rect = gui->buttonWidgets[handle.WidgetID].rect;
	button.name = gui->buttons[handle.ButtonID].name;
	button.callback = gui->buttons[handle.ButtonID].callback;

	return button;
}

GUI_Textfield GUI_getTextfield(GUI* gui, GUI_TextfieldHandle handle)
{
	GUI_Textfield textfield;

	textfield.rect = gui->buttonWidgets[handle.WidgetID].rect;
	textfield.text = gui->textfields[handle.TextfieldID].text;
	
	return textfield;
}


void GUI_setButton(GUI* gui, GUI_ButtonHandle handle, GUI_Button button)
{
	gui->buttonWidgets[handle.WidgetID].rect = button.rect;

	strcpy(gui->buttons[handle.ButtonID].name, button.name);
	gui->buttons[handle.ButtonID].callback = button.callback;
	gui->buttons[handle.ButtonID].callbackArg = button.callbackArg;
}

void GUI_setTextfield(GUI* gui, GUI_TextfieldHandle handle, GUI_Textfield textfield)
{
	gui->textfieldWidgets[handle.WidgetID].rect = textfield.rect;

	strcpy(gui->textfields[handle.TextfieldID].text, textfield.text);
}


