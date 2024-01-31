#pragma once

#include "imgui.h"
#include <memory>
#include <string>
#include <imgui_internal.h>
#include <list>
#include <vector>
#include <tuple>
#include "task.h"

#define make_window std::make_shared

class Window {
	TaskManager* task_manager = nullptr;
protected:
	std::string name;
	Task& start_task(Task&& task) {
		return task_manager->add_task(std::move(task));
	}
	ThreadTask& start_thread_task(ThreadTask&& task) {
		return task_manager->add_thread_task(std::move(task));
	}
public:
	bool visible = true;
	const char* get_name() const { return name.c_str(); }
	virtual void build() = 0;
	void set_task_manager(TaskManager& task_manager) { this->task_manager = &task_manager; }
};

using WindowHandle = std::shared_ptr<Window>;

class ImGuiDemoWindow : public Window {
public:
	ImGuiDemoWindow() {
		name = "Dear ImGui Demo";
	}

	virtual void build() override {
		ImGui::ShowDemoWindow(&visible);
	}
};

class SimpleWindow : public Window {
public:
	explicit SimpleWindow(bool& show_demo_window, bool& show_another_window, ImVec4& clear_color) {
		name = "Hello, world";
		p_show_demo_window = &show_demo_window;
		p_show_another_window = &show_another_window;
		p_clear_color = &clear_color;
	}

	virtual void build() override {
		ImGui::Begin(get_name());                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", p_show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", p_show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)p_clear_color);        // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
private:
	bool* p_show_demo_window;
	bool* p_show_another_window;
	ImVec4* p_clear_color;

	float f = 0.0f;
	int counter = 0;
};

class AnotherWindow : public Window {
public:
	AnotherWindow() {
		name = "Another Window";
	}

	virtual void build() override {
		ImGui::Begin(get_name(), &visible);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			visible = false;
		ImGui::End();
	}
};
