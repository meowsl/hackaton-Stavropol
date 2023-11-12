#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_dx9.h"
#include "../ImGui/imgui_impl_win32.h"

#include "menu.h"
#include "../globals.h"
#include "../Socket/login.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include <thread>
#include <Windows.h>
#include "../raw.h"
bool OpenFile(std::wstring& filePath) {
	OPENFILENAME ofn;
	wchar_t szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = (LPSTR)szFile;
	ofn.nMaxFile = sizeof(szFile) / sizeof(*szFile);
	ofn.lpstrFilter = "MP4 Files\0*.mp4\0All Files\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE) {
		filePath = szFile;
		return true;
	}
	else {
		return false;
	}
}
bool doOnce = false;
bool show_drag = true;

class initWindow {
public:
	const char* window_title = "sdsf";
	ImVec2 window_size{ 1024, 720 };

	DWORD window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
} iw;

void load_styles()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	{
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);

		colors[ImGuiCol_FrameBg] = ImColor(11, 11, 11, 255);
		colors[ImGuiCol_FrameBgHovered] = ImColor(11, 11, 11, 255);

		colors[ImGuiCol_Button] = ImColor(255, 0, 46, globals.button_opacity);
		colors[ImGuiCol_ButtonActive] = ImColor(255, 0, 46, globals.button_opacity);
		colors[ImGuiCol_ButtonHovered] = ImColor(255, 0, 46, globals.button_opacity);

		colors[ImGuiCol_TextDisabled] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
	}

	ImGuiStyle* style = &ImGui::GetStyle();
	{
		style->WindowPadding = ImVec2(4, 4);
		style->WindowBorderSize = 0.f;

		style->FramePadding = ImVec2(8, 6);
		style->FrameRounding = 3.f;
		style->FrameBorderSize = 1.f;
	}
}
float countdownTime = 10.0f;
bool countdownStarted;
void UpdateCountdown() {
	if (countdownStarted && countdownTime > 0.0f) {
		countdownTime -= ImGui::GetIO().DeltaTime;
	}
}

void menu::render()
{
	if (globals.active)
	{
		if (!doOnce)
		{
			load_styles();
			doOnce = true;

			
		}
		ImGui::SetNextWindowSize(iw.window_size);
		ImGui::Begin(iw.window_title, &globals.active, iw.window_flags);
		{			
			RenderLogo();
			ImGui::SetCursorPos(ImVec2(1023, 719));
			ImGui::TextDisabled("X");
			if (ImGui::IsItemClicked())
			{
				globals.active = false;
			}
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.054, 0.054, 0.054, 255));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.082, 0.078, 0.078, 255));
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.f);
			{
				ImGui::SetCursorPos(ImVec2(256, 180));
				if (!changeForm) {
					ImGui::BeginChild("##MainPanel", ImVec2(512, 360), true);
					{
						if (show_drag)
						{
							ImGui::SetCursorPos(ImVec2(50, 20));
							ImGui::TextDisabled("Flow Sentinel - Safeguarding Motion, Securing Railways");

							ImGui::SetCursorPos(ImVec2(180, 35));

							ImGui::PushItemWidth(260.f);
							{
								ImGui::SetCursorPos(ImVec2(22, 95));
								ImGui::TextDisabled("Enter file name");
								ImGui::SetCursorPos(ImVec2(290, 120));
								if (ImGui::Button("Open File Dialog")) {
									std::wstring filePath;
									if (OpenFile(filePath)) {
										// Set the selected file path back to the input field
										wcscpy((wchar_t*)globals.key, filePath.c_str());
									}
								}
								ImGui::SetCursorPos(ImVec2(20, 121));
								ImGui::InputText("##Username", globals.key, IM_ARRAYSIZE(globals.key));
							}
							ImGui::PopItemWidth();
							ImGui::SetCursorPos(ImVec2(22, 270));
							ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.f);
							if (ImGui::Button(oxorany("Load video"), ImVec2(260.f, 30.f)))
							{
								static std::thread loginThread;
								if (!loginThread.joinable())
								{
									loginThread = std::thread(SendFileToServer, globals.key);
									loginThread.detach(); 
								}
							}
							ImGui::PopStyleVar();
							ImGui::SetCursorPos(ImVec2(22, 320));
							ImGui::TextDisabled("Developed by template");


						}
					}
					ImGui::EndChild();
				}
				else {
					ImGui::BeginChild("##MainPanel", ImVec2(480, 380), true);
					{
						RenderImage();
						ImGui::Text("Analys Result = class walk");
						ImGui::Text("Recognition accuracy = 1.0");
					}
					ImGui::EndChild();

				}
			}
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar(1);
			ImGui::SetCursorPos(ImVec2(5, 700));
			ImGui::TextDisabled("Flow Sentinel");
		}
		ImGui::End();
	}
}
