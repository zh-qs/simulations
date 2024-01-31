#pragma once

#include <queue>
#include <list>
#include <chrono>
#include <memory>
#include <utility>
#include <thread>

struct TaskParameters {
	float delta_time;
};

class SingleTaskStep
{
public:
	// Executes a single step (or part of it). Returns true if step should continue in next frame, false if ended.
	virtual bool execute(const TaskParameters& parameters) = 0;
	// Executes whole step immediately.
	virtual void execute_immediately(const TaskParameters& parameters) = 0;
};

class ThreadTask
{
	friend class TaskManager;

	std::queue<std::unique_ptr<SingleTaskStep>> steps;
	bool stopped = false;
	bool paused = false;
	std::thread thread;
	const float& delta_time;
	//bool* task_ended;

	inline void execute() {
		thread = std::thread([this]() {
			while (true) {
				if (ended())
				{
					return;
				}
				if (!paused)
				{
					auto& step = steps.front();

					bool should_continue = step->execute({ delta_time });

					if (!should_continue)
						steps.pop();
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long long>(1000 * delta_time)));
			}
		});
	}
public:
	ThreadTask(const float& delta_time) : delta_time(delta_time) {}

	ThreadTask(ThreadTask&& task) noexcept : steps(std::move(task.steps)), stopped(task.stopped), paused(task.paused), thread(std::move(task.thread)), delta_time(task.delta_time) {}

	template <class T, class... Args>
	inline void add_step(Args&&... args) { steps.push(std::make_unique<T>(std::forward<Args>(args)...)); }

	inline bool ended() { return stopped || steps.empty(); }

	inline void pause() {
		paused = true;
	}

	inline void resume() {
		if (!paused) return;
		paused = false;
	}

	inline void terminate() { stopped = true; }

	~ThreadTask() {
		terminate();
		if (thread.joinable())
			thread.join();
	}
};

class Task
{
	friend class TaskManager;

	std::queue<std::unique_ptr<SingleTaskStep>> steps;
	std::chrono::time_point<std::chrono::high_resolution_clock> previous_time_point;
	bool stopped = false;
	bool paused = false;
	bool* task_ended;

	inline void execute_step() {
		if (paused)
			return;
		if (ended())
		{
			if (task_ended != nullptr)
				*task_ended = true;
			return;
		}

		auto& step = steps.front();

		auto time_point = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> time_delta = time_point - previous_time_point;
		bool should_continue = step->execute({ time_delta.count() });
		previous_time_point = std::chrono::high_resolution_clock::now();

		if (!should_continue)
			steps.pop();
	}

public:
	Task(bool& task_ended) : task_ended(&task_ended) {
		*(this->task_ended) = false;
		previous_time_point = std::chrono::high_resolution_clock::now();
	}

	Task() : task_ended(nullptr) {
		previous_time_point = std::chrono::high_resolution_clock::now();
	}

	inline void execute_immediately() {
		while (!ended())
		{
			auto time_point = std::chrono::high_resolution_clock::now();
			std::chrono::duration<float> time_delta = time_point - previous_time_point;
			steps.front()->execute_immediately({ time_delta.count() });
			previous_time_point = std::chrono::high_resolution_clock::now();
			steps.pop();
		}
		if (task_ended != nullptr)
			*task_ended = true;
	}

	template <class T, class... Args>
	inline void add_step(Args&&... args) { steps.push(std::make_unique<T>(std::forward<Args>(args)...)); }

	inline bool ended() { return stopped || steps.empty(); }

	inline void pause() {
		paused = true;
	}

	inline void resume() {
		if (!paused) return;
		paused = false;
		previous_time_point = std::chrono::high_resolution_clock::now();
	}

	inline void terminate() { stopped = true; }
};

class TaskManager
{
	friend class GlApplication;

	std::list<Task> tasks;
	std::list<ThreadTask> thread_tasks;
	inline void execute_tasks() {
		auto it = tasks.begin();

		while (it != tasks.end())
		{
			it->execute_step();
			if (it->ended())
			{
				if (it->task_ended != nullptr)
					*(it->task_ended) = true;
				it = tasks.erase(it);
			}
			else
				++it;
		}

		auto it2 = thread_tasks.begin();

		while (it2 != thread_tasks.end())
		{
			if (it2->ended())
			{
				it2->thread.join();
				it2 = thread_tasks.erase(it2);
			}
			else
				++it2;
		}
	}

public:
	inline Task& add_task(Task&& task) {
		tasks.push_back(std::move(task));
		return tasks.back();
	}

	inline ThreadTask& add_thread_task(ThreadTask&& task) {
		thread_tasks.push_back(std::move(task));
		ThreadTask& current = thread_tasks.back();
		current.execute();
		return current;
	}
};