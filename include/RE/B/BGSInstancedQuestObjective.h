#pragma once

#include "RE/B/BSString.h"
#include "RE/Q/QuestObjectiveStates.h"

namespace RE
{
	class BGSQuestObjective;

	class BGSInstancedQuestObjective
	{
	public:
		[[nodiscard]] BSString GetDisplayTextWithReplacedTags() const
		{
			using func_t = void (BGSInstancedQuestObjective::*)(BSString&) const;
			REL::Relocation<func_t> func{ RELOCATION_ID(23229, 23684) };

			BSString retVal;
			func(this, retVal);
			return retVal;
		}

		// members
		BGSQuestObjective* objective;		  // 00
		std::uint32_t instanceID;			  // 08
		QUEST_OBJECTIVE_STATE instanceState;  // 0C
	};
	static_assert(sizeof(BGSInstancedQuestObjective) == 0x10);
}
