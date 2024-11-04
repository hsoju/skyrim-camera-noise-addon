#pragma once
#include <shared_mutex>
#include <SimpleIni.h>
#include "CameraNoise_API.h"

class AnimationEventHandler
{
private:
	std::shared_mutex settings_mtx;
	std::map<std::string, std::vector<std::pair<std::string, std::string>>> load_events;
	std::map<std::string, std::vector<std::pair<std::string, std::string>>> unload_events;

	CameraNoise_API::CameraNoiseInterface* camera_api = nullptr;

public:
	std::set<std::string> loaded_inis;

	static AnimationEventHandler* GetSingleton()
	{
		static AnimationEventHandler singleton;
		return &singleton;
	}

	void ResetInis();
	void LoadIni(std::string& ini);
	void UnloadIni(std::string& ini);

	bool GetGraphVariable(std::string& graph_variable); 
	void CheckAnimation(std::string& animation_event);

	bool RequestAPI();

	void ImportAnimationEvent(CSimpleIniA& ini, const char* section, std::list<CSimpleIniA::Entry>& event_list,
		std::map<std::string, std::vector<std::pair<std::string, std::string>>>& event_map, std::string& file_name);
	void ImportConfiguration(std::string& file_path);
	bool ImportConfigurations();

protected:
	AnimationEventHandler() = default;
	AnimationEventHandler(const AnimationEventHandler&) = delete;
	AnimationEventHandler(AnimationEventHandler&&) = delete;
	virtual ~AnimationEventHandler() = default;

	auto operator=(const AnimationEventHandler&) -> AnimationEventHandler& = delete;
	auto operator=(AnimationEventHandler&&) -> AnimationEventHandler& = delete;
};