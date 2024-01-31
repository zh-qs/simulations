#pragma once

// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include <stdio.h>
#include <list>
#include <memory>
#include "window.h"
#include "task.h"
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

class GlApplication {
public:
	explicit GlApplication(int width, int height, const char* title, const ImVec4& clear_color, bool maximized);
	void run();
	void dispose();

	void add_window(const WindowHandle& window_ptr) { window_ptr->set_task_manager(task_manager);  windows.push_back(window_ptr); }

	ImVec4& get_background_color() { return clear_color; }
	ImGuiIO& get_io() { return ImGui::GetIO(); }
	GLFWwindow* main_window;

	TaskManager task_manager;
private:

	int display_w, display_h;

	std::list<WindowHandle> windows;

	ImVec4 clear_color;
};