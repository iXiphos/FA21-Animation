/*
	Copyright 2011-2020 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	main_win.c
	Win32 application entry point (main).
*/


#if (defined _WINDOWS || defined _WIN32)


#include "a3_app_utils/Win32/a3_app_application.h"
#include "a3_app_utils/Win32/a3_app_window.h"

#include "../animal3D-DemoPlayerApp.rc.h"

#pragma comment(lib, "cimgui.lib")

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "cimgui_impl.h"


//-----------------------------------------------------------------------------
// link renderer lib and respective dependencies
#define A3_RENDERER_DEFAULT
#include "../../animal3D-DemoPlayerApp/_src_win/a3_app_renderer_config.h"


//-----------------------------------------------------------------------------
// main

a3ret a3windowBeginCustomMainLoop(a3_WindowInterface* window);

HGLRC   g_GLRenderContext;
HDC     g_HDCDeviceContext;
HWND    g_hwnd;
int     g_display_w = 800;
int     g_display_h = 600;

enum A3_MESSAGE
{
	A3_MESSAGE_BEGIN = WM_USER,
	A3_HOTLOAD_COMPLETE,
	A3_USER_EXIT_DEMO,

	A3_MENU_ITEM = 1000,
};
void CreateGlContext();

a3ret a3rendererInternalInitializeExtensions();
int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	// window data
	a3_WindowInterface wnd = { 0 };
	a3_WindowClass wndClass = { 0 };
	a3_PlatformEnvironment env = { 0 };
	a3_RenderingContext renderContext = 0;
	const a3byte *wndClassName = "A3_DEMO_PLAYER_CLASS";
	const a3byte *wndName = "animal3D Demo Player";

	// some widescreen resolutions
//	const a3ui32 winWidth = 1280, winHeight = 720;
//	const a3ui32 winWidth = 1024, winHeight = 576;
	const a3ui32 winWidth =  960, winHeight = 540;

	// standard resolution
//	const a3ui32 winWidth =  480, winHeight = 360;

	// result of startup tasks
	a3i32 status = 0;

	// initialize app
	status = a3appStartSingleInstanceSwitchExisting(wndClassName, wndName);

	// register window class
	status = a3windowCreateDefaultRenderingClass(&wndClass, hInstance, wndClassName, sizeof(void *), IDI_ICON1, IDI_ICON1);
	if (status > 0)
	{
		// create rendering context
		status = a3rendererCreateDefaultContext(&renderContext, &wndClass);
		if (status > 0)
		{
			// init platform environment for debugging
			status = a3windowInitPlatformEnvironment(&env, _A3VSLAUNCHPATHSTR, _A3SLNPATHSTR, _A3SDKDIRSTR, _A3CFGDIRSTR);

			// create window
			status = a3windowCreate(&wnd, &wndClass, &env, &renderContext, wndName, winWidth, winHeight, 1, 0);


			igCreateContext(NULL);
			igStyleColorsDark(NULL);

			ImGui_ImplWin32_Init(wnd.windowHandle);
			const char* glsl_version = "#version 430";
			ImGui_ImplOpenGL3_Init(glsl_version);

			if (status > 0)
			{
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplWin32_NewFrame();
				igNewFrame();
				// main loop
				status = a3windowBeginMainLoop(&wnd);
			}

			ImGui_ImplOpenGL3_Shutdown();
			// kill rendering context
			status = a3rendererReleaseContext(&renderContext);
			// Cleanup
			
		
			igDestroyContext(NULL);
			ImGui_ImplWin32_Shutdown();


		}

		// kill window class
		status = a3windowReleaseClass(&wndClass, hInstance);
	}

	// the end
	return 0;
}


#endif	// (defined _WINDOWS || defined _WIN32)