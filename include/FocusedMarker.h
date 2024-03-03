#pragma once

#include "utils/Geometry.h"

#include "Settings.h"

struct FocusedMarker
{
	struct Data
	{
		Data(std::uint32_t a_gfxIndex, std::uint32_t a_gfxGotoFrame) :
			gfxIndex{ a_gfxIndex }, gfxGotoFrame{ a_gfxGotoFrame }
		{}

		virtual ~Data() = default;

		std::uint32_t gfxIndex;
		std::uint32_t gfxGotoFrame;
	};

	struct QuestData : Data
	{
		QuestData(std::uint32_t a_gfxIndex, std::uint32_t a_gfxGotoFrame, RE::TESObjectREFR* a_markerRef,
			const RE::TESQuest* a_quest);

		std::string GetQuestName(const RE::TESQuest* a_quest) const
		{
			RE::BSString questFullName = a_quest->GetFullName();

			ReplaceTagsInQuestText(&questFullName, a_quest, a_quest->currentInstanceID);

			return questFullName.c_str();
		}

		const std::string& GetTargetText() const
		{
			if (settings::display::showObjectiveAsTarget) {
				if (objectives.size() == 1) {
					return objectives.back();
				} else {
					static std::string markerText;
					markerText.clear();
					for (int i = 0; i < objectives.size(); i++) {
						if (i > 0) {
							markerText.append(", ");
						}
						markerText.append(objectives[i]);
					}
					return markerText;
				}
			} else {
				return locationName.empty() ? characterName : locationName;
			}
		}

		const RE::TESQuest* quest;

		// cache
		RE::QUEST_DATA::Type type = quest->GetType();
		std::string name = (type == RE::QUEST_DATA::Type::kMiscellaneous) ? "$MISCELLANEOUS" : GetQuestName(quest);
		bool isInSameLocation;
		std::string description = quest->GetCurrentDescriptionWithReplacedTags().c_str();
		std::vector<std::string> objectives;
		std::string locationName;
		std::string characterName;
		int ageIndex = -1;

		// Avoid repeating added objectives for each marker
		std::vector<const RE::BGSInstancedQuestObjective*> addedInstancedObjectives;
	};

	struct LocationData : Data
	{
		LocationData(std::uint32_t a_gfxIndex, std::uint32_t a_gfxGotoFrame, const RE::MapMarkerData* a_extraData) :
			Data{ a_gfxIndex, a_gfxGotoFrame }, extraData{ a_extraData }
		{}

		const RE::MapMarkerData* extraData;

		// cache
		std::string locationName = extraData->locationName.GetFullName();
	};

	struct EnemyData : Data
	{
		EnemyData(std::uint32_t a_gfxIndex, std::uint32_t a_gfxGotoFrame, const RE::Character* a_enemy) :
			Data{ a_gfxIndex, a_gfxGotoFrame }, enemy{ a_enemy }
		{
			auto xTextData = enemy->extraList.GetByType<RE::ExtraTextDisplayData>();
			if (xTextData) {
				enemyName = xTextData->displayName;
			} else {
				enemyName = enemy->GetName();
			}
		}

		const RE::Character* enemy;

		// cache
		std::string enemyName;
	};

	struct PlayerSetData : Data
	{
		PlayerSetData(std::uint32_t a_gfxIndex, std::uint32_t a_gfxGotoFrame) :
			Data{ a_gfxIndex, a_gfxGotoFrame }
		{}

		std::string locationName = "";
	};

	FocusedMarker(const RE::TESObjectREFR* a_markerRef, float a_angleToPlayerCamera) :
		ref{ a_markerRef }, angleToPlayerCamera{ a_angleToPlayerCamera }
	{}

	void UpdateGeometry(float a_angleToPlayerCamera)
	{
		angleToPlayerCamera = a_angleToPlayerCamera;
		distanceToPlayer = util::GetDistanceBetween(RE::PlayerCharacter::GetSingleton(), ref);
		heightDifference = util::GetHeightDifferenceBetween(RE::PlayerCharacter::GetSingleton(), ref);
	}

	const RE::TESObjectREFR* ref;
	float angleToPlayerCamera;
	float distanceToPlayer = util::GetDistanceBetween(RE::PlayerCharacter::GetSingleton(), ref);
	float heightDifference = util::GetHeightDifferenceBetween(RE::PlayerCharacter::GetSingleton(), ref);
	std::vector<std::shared_ptr<Data>> data;
};