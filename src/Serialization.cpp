#include "Serialization.h"
#include "AnimationEventHandler.h"

namespace Serialization
{
	void SaveCallback(SKSE::SerializationInterface* a_intfc)
	{
		// Do nothing
	}

	void LoadCallback(SKSE::SerializationInterface* a_intfc)
	{
		AnimationEventHandler::GetSingleton()->ResetInis();
	}

	void RevertCallback(SKSE::SerializationInterface*)
	{
		AnimationEventHandler::GetSingleton()->ResetInis();
	}
}