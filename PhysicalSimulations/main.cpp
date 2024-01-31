#include "spinning_top_view_window.h"
#include "gl_application.h"
#include "spring_view_window.h"
#include "plot_window.h"
#include "fullscreen_window.h"
#include "spring_parameters_window.h"
#include "spinning_top_simulation.h"
#include "spinning_top_parameters_window.h"
#include "animation_view_window.h"
#include "animation_parameters_window.h"
#include "jelly_view_window.h"
#include "jelly_parameters_window.h"
#include "simple_chain_view_window.h"
#include "simple_chain_parameters_window.h"
#include "puma_view_window.h"
#include "puma_parameters_window.h"
#include "flywheel_view_window.h"
#include "flywheel_parameters_window.h"

#define ANIMATION

int main(int, char**)
{
	GlApplication app(1280, 720, "Simulations", ImVec4(0.27f, 0.33f, 0.36f, 1.00f), true);

#ifdef SPRING

	SpringSimulation simulation;

	auto fullscreen = make_window<FullscreenWindow>();
	//auto demo = make_window<ImGuiDemoWindow>();
	auto spring = make_window<SpringViewWindow>(simulation);
	auto move_plot = make_window<MoveParametersPlotWindow>();
	auto trajectory = make_window<TrajectoryPlotWindow>();
	auto spring_parameters = make_window<SpringParametersWindow>(simulation);
	auto force_plot = make_window<ForcesPlotWindow>();
	auto task4_plot = make_window<Task4PlotWindow>();

	simulation.add_state_listener(*move_plot);
	simulation.add_state_listener(*trajectory);
	simulation.add_state_listener(*task4_plot);

	simulation.add_force_listener(*force_plot);
	simulation.add_force_listener(*task4_plot);

	auto& dockspace = fullscreen->get_dockspace();

	auto split = dockspace.split(ImGuiDir_Right, 0.3f);
	auto split1 = split.first->split(ImGuiDir_Down, 0.5f);
	split1.first->dock(*trajectory);
	split1.second->dock(*force_plot);
	//split1.second->dock(*demo);
	auto split2 = split.second->split(ImGuiDir_Left, 0.3f);
	split2.first->dock(*spring_parameters);
	auto split22 = split2.second->split(ImGuiDir_Down, 0.3f);
	split22.first->dock(*move_plot);
	split22.first->dock(*task4_plot);
	split22.second->dock(*spring);

	app.add_window(fullscreen);
	//app.add_window(demo);
	app.add_window(move_plot);
	app.add_window(spring);
	app.add_window(trajectory);
	app.add_window(spring_parameters);
	app.add_window(force_plot);
	app.add_window(task4_plot);

	app.task_manager.add_task(simulation.get_task());
#endif
#ifdef CUBE

	SpinningTopSimulation spin_simulation;
	auto fullscreen = make_window<FullscreenWindow>();
	auto spinning_top_view = make_window<SpinningTopViewWindow>(spin_simulation);
	auto spinning_top_parameters = make_window<SpinningTopParametersWindow>(spin_simulation, *spinning_top_view);

	spin_simulation.add_state_listener(*spinning_top_view);
	
	auto& dockspace = fullscreen->get_dockspace();
	auto split = dockspace.split(ImGuiDir_Left, 0.8f);
	split.first->dock(*spinning_top_view);
	split.second->dock(*spinning_top_parameters);

	app.add_window(fullscreen);
	app.add_window(spinning_top_view);
	app.add_window(spinning_top_parameters);

	app.task_manager.add_thread_task(spin_simulation.get_task());
#endif
#ifdef ANIMATION
	Animation animation;

	auto fullscreen = make_window<FullscreenWindow>();
	auto view1 = make_window<AnimationViewWindow>("View 1: Quaternions", animation, false);
	auto view2 = make_window<AnimationViewWindow>("View 2: Euler angles", animation, true);
	auto animation_parameters = make_window<AnimationParametersWindow>(animation, *view1, *view2);

	auto& dockspace = fullscreen->get_dockspace();
	auto split = dockspace.split(ImGuiDir_Up, 0.7f);
	split.second->dock(*animation_parameters);
	auto split1 = split.first->split(ImGuiDir_Left, 0.5f);
	split1.first->dock(*view1);
	split1.second->dock(*view2);

	app.add_window(fullscreen);
	app.add_window(animation_parameters);
	app.add_window(view1);
	app.add_window(view2);
#endif
#ifdef JELLY
	JellySimulation simulation;

	auto fullscreen = make_window<FullscreenWindow>();
	auto view = make_window<JellyViewWindow>(simulation);
	auto parameters = make_window<JellyParametersWindow>(simulation, *view);

	auto& dockspace = fullscreen->get_dockspace();
	auto split = dockspace.split(ImGuiDir_Left, 0.2f);
	split.first->dock(*parameters);
	split.second->dock(*view);

	app.add_window(fullscreen);
	app.add_window(view);
	app.add_window(parameters);

	app.task_manager.add_thread_task(simulation.get_task());
#endif
#ifdef SIMPLE_CHAIN
	SimpleChain chain;

	auto fullscreen = make_window<FullscreenWindow>();
	auto view = make_window<SimpleChainViewWindow>(chain);
	auto parameters = make_window<SimpleChainParametersWindow>(chain, *view);

	auto& dockspace = fullscreen->get_dockspace();
	auto split = dockspace.split(ImGuiDir_Left, 0.2f);
	split.first->dock(*parameters);
	split.second->dock(*view);

	app.add_window(fullscreen);
	app.add_window(view);
	app.add_window(parameters);
#endif
#ifdef PUMA
	PumaSimulation puma;

	auto fullscreen = make_window<FullscreenWindow>();
	auto view1 = make_window<PumaViewWindow>("View 1: Parameters interpolation", puma, true);
	auto view2 = make_window<PumaViewWindow>("View 2: Coordinates interpolation", puma, false);
	auto parameters = make_window<PumaParametersWindow>(puma, *view1, *view2);

	auto& dockspace = fullscreen->get_dockspace();
	auto split = dockspace.split(ImGuiDir_Up, 0.7f);
	split.second->dock(*parameters);
	auto split1 = split.first->split(ImGuiDir_Left, 0.5f);
	split1.first->dock(*view1);
	split1.second->dock(*view2);

	app.add_window(fullscreen);
	app.add_window(parameters);
	app.add_window(view1);
	app.add_window(view2);
#endif
#ifdef FLYWHEEL
	FlywheelSimulation flywheel_simulation;

	auto fullscreen = make_window<FullscreenWindow>();
	auto view = make_window<FlywheelViewWindow>(flywheel_simulation);
	auto xplot = make_window<SingleFunctionPlotWindow>("Position", "x(t)", 0);
	auto xtplot = make_window<SingleFunctionPlotWindow>("Velocity", "x'(t)", 1);
	auto xttplot = make_window<SingleFunctionPlotWindow>("Acceleration", "x''(t)", 2);
	auto trajectory = make_window<TrajectoryPlotWindow>(false);
	auto parameters = make_window<FlywheelParametersWindow>(flywheel_simulation);

	flywheel_simulation.add_listener(*xplot);
	flywheel_simulation.add_listener(*xtplot);
	flywheel_simulation.add_listener(*xttplot);
	flywheel_simulation.add_listener(*trajectory);

	auto& dockspace = fullscreen->get_dockspace();
	auto split = dockspace.split(ImGuiDir_Right, 0.3f);
	auto split_right1 = split.first->split(ImGuiDir_Up, 1.0f / 3.0f);
	auto split_right2 = split_right1.second->split(ImGuiDir_Up, 0.5f);

	auto split_left1 = split.second->split(ImGuiDir_Down, 0.3f);
	auto split_leftdown1 = split_left1.first->split(ImGuiDir_Left, 0.5f);

	split_left1.second->dock(*view);
	split_right1.first->dock(*xplot);
	split_right2.first->dock(*xtplot);
	split_right2.second->dock(*xttplot);
	split_leftdown1.first->dock(*parameters);
	split_leftdown1.second->dock(*trajectory);

	app.add_window(fullscreen);
	app.add_window(view);
	app.add_window(xplot);
	app.add_window(xtplot);
	app.add_window(xttplot);
	app.add_window(parameters);
	app.add_window(trajectory);

	app.task_manager.add_thread_task(flywheel_simulation.create_task());
#endif

	app.get_io().Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);

	app.run();

	app.dispose();

	return 0;
}
