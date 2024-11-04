#include "AnimationEventHandler.h"

auto animation_handler = AnimationEventHandler::GetSingleton();

struct Hooks
{
	static void Install()
	{
		stl::write_vfunc<0x1, AnimationEventHook>(RE::VTABLE_PlayerCharacter[2]);
		logger::info("Installed hook AnimationEvent");
	}

	struct AnimationEventHook
	{
		static RE::BSEventNotifyControl thunk(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink,
			RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource)
		{
			std::string animation_event = a_event->tag.data();
			animation_handler->CheckAnimation(animation_event);
			return func(a_sink, a_event, a_eventSource);
		}

		static inline REL::Relocation<decltype(thunk)> func;
	};
};

bool SetupAnimationHandler() {
	return animation_handler->ImportConfigurations() && animation_handler->RequestAPI();
}

bool SetupHooks() {
	Hooks::Install();
	return true;
}