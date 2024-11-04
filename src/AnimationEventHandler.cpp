#include <codecvt>
#include <SimpleIni.h>
#include "AnimationEventHandler.h"

void AnimationEventHandler::ResetInis()
{
	loaded_inis.clear();
}

void AnimationEventHandler::LoadIni(std::string& ini) {
	loaded_inis.insert(ini);
	camera_api->LoadIni(ini);
}

void AnimationEventHandler::UnloadIni(std::string& ini)
{
	loaded_inis.erase(ini);
	camera_api->UnloadIni(ini);
}

bool AnimationEventHandler::GetGraphVariable(std::string& graph_variable) {
	bool value = false;
	RE::PlayerCharacter::GetSingleton()->GetGraphVariableBool(graph_variable, value);
	return value;
}

void AnimationEventHandler::CheckAnimation(std::string& animation_event) {
	if (load_events.contains(animation_event)) {
		auto& mappings = load_events[animation_event];
		for (auto& [ini, graph_variable] : mappings) {
			if (!loaded_inis.contains(ini)) {
				if (graph_variable.size() == 0) {
					LoadIni(ini);
				} else {
					if (GetGraphVariable(graph_variable))
						LoadIni(ini);
				}
			}
		}
	} else if (unload_events.contains(animation_event)) {
		auto& mappings = unload_events[animation_event];
		for (auto& [ini, graph_variable] : mappings) {
			if (loaded_inis.contains(ini)) {
				if (graph_variable.size() == 0) {
					UnloadIni(ini);
				} else {
					if (GetGraphVariable(graph_variable))
						UnloadIni(ini);
				}
			}
		}
	}
}

bool AnimationEventHandler::RequestAPI() {
	if (!camera_api) {
		camera_api = static_cast<CameraNoise_API::CameraNoiseInterface*>(CameraNoise_API::RequestPluginAPI(
			CameraNoise_API::InterfaceVersion::V1));
		if (camera_api) {
			logger::info("Obtained CameraNoise_API");
			return true;
		} else {
			logger::info("Failed to obtain CameraNoise_API");
			return false;
		}
	}
	return false;
}

void AnimationEventHandler::ImportAnimationEvent(CSimpleIniA& ini, const char* section, std::list<CSimpleIniA::Entry>& event_list, 
	std::map<std::string, std::vector<std::pair<std::string, std::string>>>& event_map, std::string& file_name)
{
	for (auto& entry : event_list) {
		auto animation_event = std::string(entry.pItem);
		if (!event_map.contains(animation_event)) {
			event_map[animation_event] = std::vector<std::pair<std::string, std::string>>();
		}
		const char* c_animation_event = animation_event.data();
		auto graph_variable = ini.GetValue(section, c_animation_event, "");
		std::string s_graph_variable(graph_variable);
		std::pair<std::string, std::string> event_mapping(file_name, s_graph_variable);
		event_map[animation_event].push_back(event_mapping);
		if (s_graph_variable.size() > 0) {
			logger::info("{} added: {} linking {} using {}", section, animation_event, file_name, s_graph_variable);
		} else {
			logger::info("{} added: {} linking {}", section, animation_event, file_name);	
		}
	}
}

void AnimationEventHandler::ImportConfiguration(std::string& file_path)
{
	CSimpleIniA ini;
	ini.SetUnicode();
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring converted_file_path = converter.from_bytes(file_path);
	const wchar_t* w_file_path = converted_file_path.c_str();
	ini.LoadFile(w_file_path);

	auto file_name = std::filesystem::path(file_path).filename().string();

	std::list<CSimpleIniA::Entry> current_load_events;
	const char* load_section = "LoadAnimationEvents";
	ini.GetAllKeys(load_section, current_load_events);
	ImportAnimationEvent(ini, load_section, current_load_events, load_events, file_name);

	std::list<CSimpleIniA::Entry> current_unload_events;
	const char* unload_section = "UnloadAnimationEvents";
	ini.GetAllKeys(unload_section, current_unload_events);
	ImportAnimationEvent(ini, unload_section, current_unload_events, unload_events, file_name);
}

bool AnimationEventHandler::ImportConfigurations() {
	std::string directory_path = "Data\\SKSE\\Plugins\\_CameraNoise";
	std::vector<std::string> file_paths;
	for (const auto& entry : std::filesystem::directory_iterator(directory_path)) {
		std::string file_path = entry.path().string();
		file_paths.push_back(file_path);
	}
	std::lock_guard<std::shared_mutex> lk(settings_mtx);
	for (auto& file_path : file_paths) {
		ImportConfiguration(file_path);
	}
	return load_events.size() > 0 || unload_events.size() > 0;
}