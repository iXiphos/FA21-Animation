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

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
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
bool SetSwapInterval(int interval);

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

			/*
			create imgui window

			*/
			const a3byte* wndClassNameImgui = "A3_DEMO_PLAYER_CLASS_IMGUI";
			const a3byte* wndNameImgui = "animal3D Demo Player Imgui";
			WNDCLASSA wc = { 0 };
			wc.lpfnWndProc = WndProc;
			wc.hInstance = hInstance;
			wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
			wc.lpszClassName = "NCUI";
			wc.style = CS_OWNDC;
			if (!RegisterClassA(&wc))
				return 1;
			g_hwnd = CreateWindowA(wc.lpszClassName, "teste", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 640, 480, 0, 0, hInstance, 0);


			ShowWindow(g_hwnd, SW_SHOWDEFAULT);
			UpdateWindow(g_hwnd);

			ImVec4 clear_color = { 1.000F, 1.000F, 1.000F, 1.0F };

			CreateGlContext();
			// It seems like we dont need to initialize opengl extentions twice, 
			// if we actually do, make sure to change the function to let glwInit run twice
			//a3rendererInternalInitializeExtensions();

			igCreateContext(NULL);
			igStyleColorsDark(NULL);

			ImGui_ImplWin32_Init(g_hwnd);
			const char* glsl_version = "#version 430";
			ImGui_ImplOpenGL3_Init(glsl_version);

			if (status > 0)
			{
				// main loop
				//status = a3windowBeginMainLoop(&wnd);
				status = a3windowBeginCustomMainLoop(&wnd);
			}

			// kill rendering context
			status = a3rendererReleaseContext(&renderContext);
		}

		// kill window class
		status = a3windowReleaseClass(&wndClass, hInstance);
	}

	// the end
	return 0;
}

void CreateGlContext() {

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
		PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
		32,                   // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,                   // Number of bits for the depthbuffer
		8,                    // Number of bits for the stencilbuffer
		0,                    // Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	g_HDCDeviceContext = GetDC(g_hwnd);

	int pixelFormal = ChoosePixelFormat(g_HDCDeviceContext, &pfd);
	SetPixelFormat(g_HDCDeviceContext, pixelFormal, &pfd);
	g_GLRenderContext = wglCreateContext(g_HDCDeviceContext);
	wglMakeCurrent(g_HDCDeviceContext, g_GLRenderContext);
}

extern a3boolean a3rendererInternalContextIsCurrent(const a3_RenderingContext renderingContext);
a3ret a3windowBeginCustomMainLoop(a3_WindowInterface* window) {
	// message
	MSG msg[1] = { 0 };

	// idle result
	a3i32 idle;
	//bool show_demo_window;
	// while quit message has not been posted
	while (msg->message - WM_QUIT)
	{	
		// check for message
		if (PeekMessage(msg, NULL, 0, 0, PM_REMOVE))
		{
			// if there is a message, process the message
			TranslateMessage(msg);
			DispatchMessage(msg);
		}

		// if no message, idle (window required)
		else if (window)
		{
			idle = window->demo->callbacks->callback_idle(window->demo->data);
	
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplWin32_NewFrame();
			igNewFrame();
			igShowDemoWindow(NULL);
			igRender();       
      
			wglMakeCurrent(g_HDCDeviceContext, g_GLRenderContext);
			//glViewport(0, 0, g_display_w, g_display_h);
			ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
			wglMakeCurrent(g_HDCDeviceContext, g_GLRenderContext);
			SwapBuffers(g_HDCDeviceContext);
			
			// if the result is positive, idle is successful
			// if rendering, this should mean that a frame was rendered
			if (idle > 0)
			{
				if (a3rendererInternalContextIsCurrent(window->renderingContext))
				{
					// swap buffers
					SwapBuffers(window->deviceContext);
				}
			}

			// if the result is negative, the demo should be unloaded
			// standalone window should close the window, which also unloads
			else if (idle < 0)
			{
				if (window->isStandalone)
				{
					// standalone mode, kill window
					PostMessageA(window->windowHandle, WM_CLOSE, 0, 0);
				}
				else
				{
					// exit demo
					PostMessageA(window->windowHandle, A3_USER_EXIT_DEMO, 0, 0);
				}
			}
			
			// if result is zero, nothing happened
			// ...carry on
		}
		
	}
	return (a3i32)msg->wParam;
}


// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		/*
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		*/
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

#endif	// (defined _WINDOWS || defined _WIN32)