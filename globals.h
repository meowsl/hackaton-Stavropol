#pragma once

#include "ImGui/ImGui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"
bool injected;
class c_globals {
public:
	bool active = true;
	
	char key[255] = "";

	char invite_key[255] = "Enter your invite key";

	int selectedClass;  

	int button_opacity = 255;

} globals;