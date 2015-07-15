#include "Editor.h"
#include "DebugDraw.h"



void initEditor(EditorData* data)
{


	GUI_initGUI(&data->gui);

	data->view.position = { 0.0f, 0.0f };
	data->view.scale = { 1.0f, 1.0f };
	data->view.angle = 0.0f;

	Bone firstBone;
	firstBone.jointPos = { 0, 0 };
	firstBone.jointAngle = { 1, 0 };
	firstBone.name = 0;
	firstBone.parentJoint = -1;

	Constraint constraint;
	constraint.minAngle =  0;
	constraint.maxAngle = 2 * PI;

	data->skeleton.push(firstBone);
	data->bones.push({ { 400 - 5, 300 - 5, 10, 10 } });
	data->names.push({ "test" });
	data->constraints.push(constraint);

	data->grabbedBone = -1;
	data->newBone = -1;


	memset(data->skeletonName, '\0', 64);
	memset(data->frameName, '\0', 64);
}

void shutdownEditor(EditorData* data)
{
	GUI_shutdownGUI(data->gui);
}


void updateBones(EditorData* data)
{
	data->bones.clear();

	floatRect rect{ 400 + data->skeleton[0].jointPos.x - 5, 300 + data->skeleton[0].jointPos.y - 5, 10, 10 };
	data->bones.push({ rect });

	//float* angles = new float[data->skeleton.size()];
	float2* angles = new float2[data->skeleton.size()];
	angles[0] = data->skeleton[0].jointAngle;

	for (int j = 1; j < data->skeleton.size(); ++j)
	{
		rect = data->bones[data->skeleton[j].parentJoint].rect;
		rect.x += rotate(data->skeleton[j].jointPos, angles[data->skeleton[j].parentJoint]).x;
		rect.y += rotate(data->skeleton[j].jointPos, angles[data->skeleton[j].parentJoint]).y;
		angles[j] = rotate(angles[data->skeleton[j].parentJoint], data->skeleton[j].jointAngle);
		data->bones.push({ rect });
	}

	delete[] angles;
}


void updateEditor(SDL_Event event, EditorData* data)
{
	
	GUI_sendEventToGUI(data->gui, &event);

	const unsigned char* keyboardState = SDL_GetKeyboardState(NULL);
	int2 mousePixel;
	SDL_GetMouseState(&mousePixel.x, &mousePixel.y);

	float2 mousePos = pixelToPoint(&data->view, mousePixel);


	updateBones(data);

	switch (event.type)
	{
	case SDL_KEYDOWN:
	{

		switch (event.key.keysym.scancode)
		{
		case SDL_SCANCODE_S:
			if (event.key.keysym.mod == KMOD_LCTRL)
			{
				SDL_Event lastEvent = data->lowEventStack.Top();

				if (lastEvent.key.keysym.scancode == SDL_SCANCODE_S && lastEvent.key.keysym.mod == KMOD_LCTRL)
				{
					// save skeleton

					GUI_WidgetID widget = GUI_addContainer(data->gui, GUI_MAIN_WIDGET, { 400 - 100, 300 - 25, 200, 50}, GUI_Container::Orientation::HORIZONTAL);

					auto callbackSave = [](GUI* gui, GUI_WidgetID widgetID, void* editorData)
					{
						EditorData* data = (EditorData*)editorData;

						GUI_WidgetID widget = GUI_getParentWidget(gui, widgetID);

						const char* filename = GUI_getTextfield(gui, GUI_getChildWidget(gui, widget, 0)).text;

						saveSkeleton(filename, &data->skeleton[0], &data->names[0], data->skeleton.size());

						GUI_removeWidget(gui, widget);
					};

					auto callbackCancel = [](GUI* gui, GUI_WidgetID widgetID, void*)
					{
						GUI_WidgetID widget = GUI_getParentWidget(gui, widgetID);
						GUI_removeWidget(gui, widget);
					};

					GUI_addTextfield(data->gui, widget, { 0, 0, -1, -1 }, "filename");
					GUI_addButton(data->gui, widget, { 0, 0, 50, -1 }, "Save", data, callbackSave);
					GUI_addButton(data->gui, widget, { 0, 0, 50, -1 }, "Cancel", 0, callbackCancel);

					break;
				}
				else if (lastEvent.key.keysym.scancode == SDL_SCANCODE_O && lastEvent.key.keysym.mod == KMOD_LCTRL)
				{
					// load skeleton

					GUI_WidgetID widget = GUI_addContainer(data->gui, GUI_MAIN_WIDGET, { 400 - 100, 300 - 15, 200, 30 }, GUI_Container::Orientation::HORIZONTAL);


					auto callbackLoad = [](GUI* gui, GUI_WidgetID widgetID, void* editorData)
					{
						EditorData* data = (EditorData*)editorData;

						GUI_WidgetID widget = GUI_getParentWidget(gui, widgetID);

						const char* filename = GUI_getTextfield(gui, GUI_getChildWidget(gui, widget, 0)).text;

						strcpy(data->skeletonName, filename);
						
						const SkeletonSave* save = loadSkeleton(filename);

						if (save != 0)
						{
							data->bones.clear();
							data->skeleton.clear();
							data->constraints.clear();
							data->names.clear();

							const char* name = save->names;
							for (int i = 0; i < save->numBones; ++i)
							{
								Bone newBone;
								newBone.jointAngle = { 1, 0 };
								newBone.jointPos = save->jointPos[i];
								newBone.parentJoint = save->parentJoints[i];
								newBone.name = data->names.push({ { 0 } });

								memcpy(data->names[newBone.name].string, name, save->nameLen[i] * sizeof(char));
								name += save->nameLen[i];

								Constraint constraint;
								constraint.minAngle = -PI;
								constraint.maxAngle = 3 * PI;

								data->skeleton.push(newBone);
								data->constraints.push(constraint);
							}

							updateBones(data);
						}

						GUI_removeWidget(gui, widget);
					};

					auto callbackCancel = [](GUI* gui, GUI_WidgetID widgetID, void*)
					{
						GUI_WidgetID widget = GUI_getParentWidget(gui, widgetID);
						GUI_removeWidget(gui, widget);
					};

					GUI_addTextfield(data->gui, widget, { 0, 0, -1, -1 }, "filename");
					GUI_addButton(data->gui, widget, { 0, 0, 50, -1 }, "Load", data, callbackLoad);
					GUI_addButton(data->gui, widget, { 0, 0, 50, -1 }, "Cancel", 0, callbackCancel);



					break;
				}


				data->lowEventStack.Push(event);
			}


			break;
			case SDL_SCANCODE_K:
				if (event.key.keysym.mod == KMOD_LCTRL)
				{
					SDL_Event lastEvent = data->lowEventStack.Top();

					if (lastEvent.key.keysym.scancode == SDL_SCANCODE_S && lastEvent.key.keysym.mod == KMOD_LCTRL)
					{
						// save keyFrame

						GUI_WidgetID widget = GUI_addContainer(data->gui, GUI_MAIN_WIDGET, { 400 - 100, 300 - 15, 200, 30 }, GUI_Container::Orientation::HORIZONTAL);

						auto callbackSave = [](GUI* gui, GUI_WidgetID widgetID, void* editorData)
						{
							EditorData* data = (EditorData*)editorData;

							GUI_WidgetID widget = GUI_getParentWidget(gui, widgetID);

							const char* filename = GUI_getTextfield(gui, GUI_getChildWidget(gui, widget, 0)).text;

							saveKeyFrame(filename, &data->skeleton[0], &data->names[0], data->skeleton.size());

							GUI_removeWidget(gui, widget);
						};

						auto callbackCancel = [](GUI* gui, GUI_WidgetID widgetID, void*)
						{
							GUI_WidgetID widget = GUI_getParentWidget(gui, widgetID);
							GUI_removeWidget(gui, widget);
						};

						GUI_addTextfield(data->gui, widget, { 0, 0, -1, -1 }, "filename");
						GUI_addButton(data->gui, widget, { 0, 0, 50, -1 }, "Save", data, callbackSave);
						GUI_addButton(data->gui, widget, { 0, 0, 50, -1 }, "Cancel", 0, callbackCancel);

						break;
					}
					else if (lastEvent.key.keysym.scancode == SDL_SCANCODE_O && lastEvent.key.keysym.mod == KMOD_LCTRL)
					{
						// load keyframe

						GUI_WidgetID widget = GUI_addContainer(data->gui, GUI_MAIN_WIDGET, { 400 - 100, 300 - 15, 200, 30 }, GUI_Container::Orientation::HORIZONTAL);


						auto callbackLoad = [](GUI* gui, GUI_WidgetID widgetID, void* editorData)
						{
							EditorData* data = (EditorData*)editorData;

							GUI_WidgetID widget = GUI_getParentWidget(gui, widgetID);

							const char* filename = GUI_getTextfield(gui, GUI_getChildWidget(gui, widget, 0)).text;

							strcpy(data->frameName, filename);

							const KeyFrameSave* save = loadKeyFrame(filename);

							if (save != 0)
							{

								const char* name = save->names;
								for (int i = 0; i < save->numBones; ++i)
								{
									int nameID = -1;
									for (int j = 0; j < data->names.size(); ++j)
									{
										if (strncmp(name, data->names[j].string, save->nameLen[i]) == 0)
										{
											nameID = j;
											break;
										}
									}

									if (nameID != -1)
									{

										if (data->skeleton[nameID].name == nameID)
										{
											data->skeleton[nameID].jointAngle = save->jointAngles[i];
										}
										else
										{
											for (int j = 0; j < data->skeleton.size(); j++)
											{
												if (data->skeleton[j].name = nameID)
													data->skeleton[j].jointAngle = save->jointAngles[i];
											}
										}
									}

									name += save->nameLen[i];
								}

								updateBones(data);
							}

							GUI_removeWidget(gui, widget);
						};

						auto callbackCancel = [](GUI* gui, GUI_WidgetID widgetID, void*)
						{
							GUI_WidgetID widget = GUI_getParentWidget(gui, widgetID);
							GUI_removeWidget(gui, widget);
						};

						GUI_addTextfield(data->gui, widget, { 0, 0, -1, -1 }, "filename");
						GUI_addButton(data->gui, widget, { 0, 0, 50, -1 }, "Load", data, callbackLoad);
						GUI_addButton(data->gui, widget, { 0, 0, 50, -1 }, "Cancel", 0, callbackCancel);


						break;
					}


					data->lowEventStack.Push(event);
				}


				break;
		default:
			data->lowEventStack.Push(event);
			break;

		}
		break;
	}
	case SDL_MOUSEBUTTONDOWN:
	{
		data->newBone = -1;

		if (keyboardState[SDL_SCANCODE_A])
		{
			for (int i = 0; i < data->bones.size(); ++i)
			{
				if (pointInRect(mousePos, data->bones[i].rect))
				{
					int name = data->names.push({ "test" });

					Bone newBone;
					newBone.jointAngle = { 1, 0 };
					newBone.jointPos = { 0, 0 };
					newBone.parentJoint = i;
					newBone.name = name;

					Constraint constraint;
					constraint.minAngle = -PI;
					constraint.maxAngle = 3 * PI;

					int newBoneID = data->skeleton.push(newBone);
					data->constraints.push(constraint);

					int* ids = new int[data->skeleton.size()];

					sortSkeleton(&data->skeleton[0], data->skeleton.size(), ids);

					Constraint* tempConstraints = new Constraint[data->constraints.size()];
					memcpy(tempConstraints, &data->constraints[0], sizeof(Constraint) * data->constraints.size());

					for (int i = 0; i < data->constraints.size(); ++i)
					{
						data->constraints[i] = tempConstraints[ids[i]];
					}

					delete[] tempConstraints;

					//find bone again
					data->newBone = ids[newBoneID];

					// rebuild bone selection

					updateBones(data);

					delete[]ids;


					break;
				}



			}
		}
		else if (keyboardState[SDL_SCANCODE_N])
		{

			for (int i = 0; i < data->bones.size(); ++i)
			{
				if (pointInRect(mousePos, data->bones[i].rect))
				{
					
					int2 pos = pointToPixel(&data->view, float2{ data->bones[i].rect.x + 10, data->bones[i].rect.y });
					
					GUI_WidgetID widget = GUI_addContainer(data->gui, GUI_MAIN_WIDGET, intRect{ pos.x, pos.y, 100, 30 }, GUI_Container::HORIZONTAL);

					struct Args
					{
						EditorData* data;
						int boneId;
					}*args = new Args{ data, i };
						

					auto callbackOk = [](GUI* gui, GUI_WidgetID widgetID, void* args)
					{
						EditorData* data = ((Args*)args)->data;
						int boneID = ((Args*)args)->boneId;

						GUI_WidgetID parentID = GUI_getParentWidget(gui, widgetID);

						const char* newName = GUI_getTextfield(gui, GUI_getChildWidget(gui, parentID, 0)).text;

						strcpy(data->names[data->skeleton[boneID].name].string, newName);

						GUI_removeWidget(gui, parentID);

						delete args;
					};

					GUI_addTextfield(data->gui, widget, intRect{ 0, 0, -1, -1 }, data->names[data->skeleton[i].name].string);
					GUI_addButton(data->gui, widget, intRect{ 0, 0, 30, -1 }, "OK", args, callbackOk);

					break;
				}
			}
		}
		else
		{



			for (int i = 0; i < data->bones.size(); i++)
			{
				if (pointInRect(mousePos, data->bones[i].rect))
				{
					data->selectedBones.push(i);
					data->grabbedBone = i;
				}
			}

			if (data->grabbedBone == -1 && !keyboardState[SDL_SCANCODE_LCTRL])
				data->selectedBones.clear();
		}

		break;
	}
	case SDL_MOUSEBUTTONUP:
	{

		data->grabbedBone = -1;
		break;
	}
	case SDL_MOUSEMOTION:
	{

		

		if (SDL_GetMouseState(0, 0) == SDL_BUTTON_MIDDLE)
		{
			data->view.position.x -= (event.motion.xrel / data->view.scale.x);
			data->view.position.y -= (event.motion.yrel / data->view.scale.y);

		}
		else
		{

			float2 motion = rotate({ (float)event.motion.xrel, (float)event.motion.yrel }, data->view.angle) * float2{ 1 / data->view.scale.x, 1 / data->view.scale.y };

			if (data->newBone != -1)
			{
				float2 angle = getBoneWorldTransform(&data->skeleton[0], data->skeleton.size(), data->newBone).angle;

				float2 rel = rotate(float2{ motion.x, motion.y }, negateRotation(angle));

				data->skeleton[data->newBone].jointPos += rel;
			}


			if (data->grabbedBone != -1)
			{
				if (keyboardState[SDL_SCANCODE_LCTRL])
				{
					float2 angle = getBoneWorldTransform(&data->skeleton[0], data->skeleton.size(), data->skeleton[data->grabbedBone].parentJoint).angle;

					float2 rel = rotate(float2{ motion.x, motion.y }, negateRotation(angle));

					data->skeleton[data->grabbedBone].jointPos += rel;
				}
				else
				{
					animateCCDIKSelection(&data->skeleton[0], &data->constraints[0], data->skeleton.size(),
						&data->selectedBones[0], data->selectedBones.size(), data->grabbedBone, mousePos - float2{ 400, 300 });
				}
			}

		}

		break;
	}
	case SDL_MOUSEWHEEL:
	{
		data->view.scale.x += event.wheel.y*0.1f;
		data->view.scale.y += event.wheel.y*0.1f;

		break;
	}
	}


	
}


void renderEditor(SDL_Renderer* renderer, EditorData* data)
{
	drawSkeleton(renderer, &data->view, { 400, 300 }, 0, &data->skeleton[0], data->skeleton.size());

	for (int i = 0; i < data->selectedBones.size(); ++i)
	{
		//SDL_RenderFillRect(renderer, (SDL_Rect*)&data->bones[data->selectedBones[i]].rect);

		fillRect(renderer, &data->view, data->bones[data->selectedBones[i]].rect);
	}


	GUI_drawGUI(data->gui, renderer);

}

