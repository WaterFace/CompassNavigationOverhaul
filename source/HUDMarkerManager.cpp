#include "HUDMarkerManager.h"

#include "RE/B/BSTimer.h"

#include "RE/I/IMenu.h"

namespace extended
{
	struct QuestTarget : RE::TESQuestTarget
	{
		RE::BSTArray<void*> unk00;
		RE::BSTArray<void*> unk18;
		RE::NiPoint3 unk30;
		RE::NiPoint3 targetPosition;
	};

	void HUDMarkerManager::ProcessQuestMarker(RE::TESQuest* a_quest, RE::BGSInstancedQuestObjective& a_instancedObjective, RE::TESQuestTarget* a_target,
		RE::TESObjectREFR* a_marker, std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCamera = GetAngleBetween(playerCamera, a_marker);

		if ((IsFocusedMarker(a_marker) && angleToPlayerCamera < keepFocusedAngle) ||
			angleToPlayerCamera < facingAngle) {
			std::shared_ptr<FocusedMarker> facedMarker = GetFacedMarkerUpdated(a_marker, angleToPlayerCamera);

			// For this marker, find the data for this quest
			auto dataFound = std::ranges::find_if(facedMarker->data,
				[a_quest](std::shared_ptr<FocusedMarker::Data> a_data) -> bool {
					auto questData = std::dynamic_pointer_cast<FocusedMarker::QuestData>(a_data);

					// Group miscellaneous quests as a single quest, like in the journal
					if (questData->type == RE::QUEST_DATA::Type::kMiscellaneous) {
						return a_quest->GetType() == RE::QUEST_DATA::Type::kMiscellaneous;
					} else {
						return questData->quest == a_quest;
					}
				});

			// Get if found, create if not
			std::shared_ptr<FocusedMarker::QuestData> questData;
			if (dataFound != facedMarker->data.end()) {
				questData = std::dynamic_pointer_cast<FocusedMarker::QuestData>(*dataFound);
				questData->gfxIndex = hudMarkerManager->currentMarkerIndex - 1;
			} else {
				questData = std::make_shared<FocusedMarker::QuestData>(
					hudMarkerManager->currentMarkerIndex - 1,
					a_markerGotoFrame,
					a_marker,
					a_quest);
				facedMarker->data.push_back(questData);
			}

			// Add objectives to the quest data. The objectives are in oldest-to-newest order,
			// so we iterate from newest-to-oldest to have it in the same order as in the journal
			for (int i = player->GetPlayerRuntimeData().objectives.size() - 1; i >= 0; i--) {
				const RE::BGSInstancedQuestObjective& instancedObjective = player->GetPlayerRuntimeData().objectives[i];

				bool objectiveHasTarget = false;
				for (int j = 0; j < instancedObjective.objective->numTargets; j++) {
					if (a_target == instancedObjective.objective->targets[j]) {
						objectiveHasTarget = true;
						break;
					}
				}

				if (instancedObjective.objective->ownerQuest == a_quest &&
					instancedObjective.instanceState == RE::QUEST_OBJECTIVE_STATE::kDisplayed && objectiveHasTarget) {
					// Add each objective only once per quest
					if (std::ranges::find(questData->addedInstancedObjectives, &instancedObjective) == questData->addedInstancedObjectives.end()) {
						//if (questData->ageIndex == -1) {
						//	questData->ageIndex = i;
						//}

						questData->addedInstancedObjectives.push_back(&instancedObjective);
						questData->objectives.push_back(instancedObjective.GetDisplayTextWithReplacedTags().c_str());
					}
				}
			}
			if (questData->addedInstancedObjectives.empty()) {
				logger::error("questData->addedInstancedObjectives is empty. This shouldn't happen");
			}
		}
	}

	void HUDMarkerManager::ProcessLocationMarker(RE::ExtraMapMarker* a_mapMarker, RE::TESObjectREFR* a_marker,
		std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCamera = GetAngleBetween(playerCamera, a_marker);

		bool isDiscoveredLocation = a_mapMarker->mapData->flags.all(RE::MapMarkerData::Flag::kVisible);

		if (isDiscoveredLocation || !settings::display::undiscoveredMeansUnknownInfo) {
			if ((IsFocusedMarker(a_marker) && angleToPlayerCamera < keepFocusedAngle) ||
				angleToPlayerCamera < facingAngle) {
				std::shared_ptr<FocusedMarker> facedMarker = GetFacedMarkerUpdated(a_marker, angleToPlayerCamera);

				// Add the location data for this marker
				auto locationData = std::make_shared<FocusedMarker::LocationData>(
					hudMarkerManager->currentMarkerIndex - 1,
					a_markerGotoFrame,
					a_mapMarker->mapData);

				facedMarker->data.push_back(locationData);
			}
		}

		// 0 is used as the sentinel value for an unknown compass marker in the most up-to-date
		// swf at time of writing
		if (!isDiscoveredLocation && settings::display::undiscoveredMeansUnknownMarkers) {
			hudMarkerManager->scaleformMarkerData[hudMarkerManager->currentMarkerIndex - 1].type.SetNumber(0);
		}
	}

	void HUDMarkerManager::ProcessEnemyMarker(RE::Character* a_enemy, std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCamera = GetAngleBetween(playerCamera, a_enemy);

		if ((IsFocusedMarker(a_enemy) && angleToPlayerCamera < keepFocusedAngle) ||
			angleToPlayerCamera < facingAngle) {
			std::shared_ptr<FocusedMarker> facedMarker = GetFacedMarkerUpdated(a_enemy, angleToPlayerCamera);

			// Add the enemy data for this marker
			auto enemyData = std::make_shared<FocusedMarker::EnemyData>(
				hudMarkerManager->currentMarkerIndex - 1,
				a_markerGotoFrame,
				a_enemy);

			facedMarker->data.push_back(enemyData);
		}
	}

	void HUDMarkerManager::ProcessPlayerSetMarker(RE::TESObjectREFR* a_marker, std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCamera = GetAngleBetween(playerCamera, a_marker);

		if ((IsFocusedMarker(a_marker) && angleToPlayerCamera < keepFocusedAngle) ||
			angleToPlayerCamera < facingAngle) {
			std::shared_ptr<FocusedMarker> facedMarker = GetFacedMarkerUpdated(a_marker, angleToPlayerCamera);

			// Add the player set data for this marker
			auto playerSetData = std::make_shared<FocusedMarker::PlayerSetData>(
				hudMarkerManager->currentMarkerIndex - 1,
				a_markerGotoFrame);

			facedMarker->data.push_back(playerSetData);
		}
	}

	void HUDMarkerManager::SetMarkersExtraInfo()
	{
		std::shared_ptr<FocusedMarker> nextFocusedMarker = GetMostCenteredMarkerOf(facedMarkers);

		facedMarkers.clear();

		bool focusChanged = !IsSameFocusedMarker(nextFocusedMarker);

		focusedMarker = nextFocusedMarker;

		if (focusChanged) {
			timeFocusingMarker = 0.0F;

			compass->UnfocusMarker();
		}

		if (focusChanged || (settings::questlist::hideInCombat && player->AsActorState()->IsWeaponDrawn())) {
			if (questItemList)
				questItemList->RemoveAllQuests();
		}

		if (focusedMarker) {
			bool canFocusPlayerSetMarker = true;
			uint32_t gfxIndex = 0;

			if (focusedMarker->data.empty()) {
				logger::trace("");
			}

			for (std::shared_ptr<FocusedMarker::Data> focusedMarkerData : focusedMarker->data) {
				auto markerIndex = focusedMarkerData->gfxIndex;
				if (auto questData = std::dynamic_pointer_cast<FocusedMarker::QuestData>(focusedMarkerData)) {
					canFocusPlayerSetMarker = false;

					compass->SetFocusedMarkerInfo(questData->GetTargetText(), focusedMarker->distanceToPlayer, focusedMarker->heightDifference, markerIndex);

					if (focusChanged && questItemList && questItemList->CanBeDisplayedIn(player->GetParentCell())) {
						questItemList->AddQuest(questData->type, questData->name, questData->isInSameLocation, questData->objectives, questData->ageIndex);
						questItemList->SetQuestSide(GetSideInQuest(questData->type));
					}
				} else if (auto locationData = std::dynamic_pointer_cast<FocusedMarker::LocationData>(focusedMarkerData)) {
					compass->SetFocusedMarkerInfo(locationData->locationName, focusedMarker->distanceToPlayer, focusedMarker->heightDifference, markerIndex);
				} else if (auto enemyData = std::dynamic_pointer_cast<FocusedMarker::EnemyData>(focusedMarkerData)) {
					compass->SetFocusedMarkerInfo(enemyData->enemyName, focusedMarker->distanceToPlayer, focusedMarker->heightDifference, markerIndex);
				} else if (auto playerSetData = std::dynamic_pointer_cast<FocusedMarker::PlayerSetData>(focusedMarkerData)) {
					if (canFocusPlayerSetMarker) {
						compass->SetFocusedMarkerInfo(playerSetData->locationName, focusedMarker->distanceToPlayer, focusedMarker->heightDifference, markerIndex);
					}
				}

				gfxIndex = focusedMarkerData->gfxIndex;
			}

			if (focusChanged) {
				compass->FocusMarker(gfxIndex);
			} else {
				// HACK: properly support hiding enemy healthbar with TrueHUD or IHUD
				// Ideally I would redistribute a modified version of the swf but this is
				// unfortunately better for compatibility and license reasons

				// Explanation: TrueHUD is commonly used to hide the vanilla enemy healthbar
				// and the way it does so is by setting the `HUDMovieBaseInstance.EnemyHealth_mc._alpha` field to 0
				// however the CNO .swf instead checks `HUDMenu.EnemyHealth_mc.BracketsInstance._alpha`, so
				// it behaves as if the enemy health bar is enabled even when it's disabled by TrueHUD or IHUD

				// This solution is not ideal but it should work

				auto hudPtr = RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME);
				if (hudPtr) {
					auto hud = hudPtr.get();
					if (hud && hud->uiMovie) {
						RE::GFxValue enemyHealthAlpha;
						hud->uiMovie->GetVariable(&enemyHealthAlpha, "HUDMovieBaseInstance.EnemyHealth_mc._alpha");
						hud->uiMovie->SetVariable("HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance._alpha", enemyHealthAlpha);
					}
				}
				compass->UpdateFocusedMarker();
			}
		}

		// Set smaller the size of non-focused markers
		compass->SetMarkers();

		if (focusedMarker && (!settings::questlist::hideInCombat || !player->AsActorState()->IsWeaponDrawn())) {
			timeFocusingMarker += timeManager->realTimeDelta;

			if (questItemList && questItemList->CanBeDisplayedIn(player->GetParentCell())) {
				float delayToShow;
				if (player->AsActorState()->DoGetMovementSpeed() < player->GetWalkSpeed()) {
					delayToShow = settings::questlist::walkingDelayToShow;
				} else if (player->AsActorState()->DoGetMovementSpeed() < player->GetJogSpeed()) {
					delayToShow = settings::questlist::joggingDelayToShow;
				} else {
					delayToShow = settings::questlist::sprintingDelayToShow;
				}

				if (timeFocusingMarker > delayToShow) {
					questItemList->ShowAllQuests();
				}
			}
		}

		if (questItemList && questItemList->CanBeDisplayedIn(player->GetParentCell())) {
			questItemList->Update();
		}
	}

	std::shared_ptr<FocusedMarker>
		HUDMarkerManager::GetMostCenteredMarkerOf(const std::unordered_map<const RE::TESObjectREFR*, std::shared_ptr<FocusedMarker>>& a_facedMarkers)
	{
		float closestAngleToPlayerCamera = std::numeric_limits<float>::max();
		std::shared_ptr<FocusedMarker> mostCenteredMarker = nullptr;

		for (const auto& [markerRef, facedMarker] : a_facedMarkers) {
			if (facedMarker->angleToPlayerCamera < closestAngleToPlayerCamera) {
				mostCenteredMarker = facedMarker;
				closestAngleToPlayerCamera = facedMarker->angleToPlayerCamera;
			}
		}

		return mostCenteredMarker;
	}

	bool HUDMarkerManager::IsSameFocusedMarker(std::shared_ptr<FocusedMarker> a_nextFocusedMarker) const
	{
		if ((focusedMarker && !a_nextFocusedMarker) || (!focusedMarker && a_nextFocusedMarker)) {
			return false;
		} else if (focusedMarker && a_nextFocusedMarker) {
			if (focusedMarker->ref != a_nextFocusedMarker->ref) {
				return false;
			}
		}

		return true;
	}

	float HUDMarkerManager::GetAngleBetween(const RE::PlayerCamera* a_playerCamera, const RE::TESObjectREFR* a_marker) const
	{
		float angleToPlayerCameraInRadians = util::GetAngleBetween(a_playerCamera, a_marker);
		float angleToPlayerCamera = util::RadiansToDegrees(angleToPlayerCameraInRadians);

		if (angleToPlayerCamera > 180.0F)
			angleToPlayerCamera = 360.0F - angleToPlayerCamera;

		return angleToPlayerCamera;
	}

	bool HUDMarkerManager::IsPlayerAllyOfFaction(const RE::TESFaction* a_faction) const
	{
		if (player->IsInFaction(a_faction)) {
			return true;
		}

		return player->VisitFactions([a_faction](RE::TESFaction* a_visitedFaction, std::int8_t a_rank) -> bool {
			if (a_visitedFaction == a_faction && a_rank > 0) {
				return true;
			}

			for (RE::GROUP_REACTION* reactionToFaction : a_visitedFaction->reactions) {
				auto relatedFaction = reactionToFaction->form->As<RE::TESFaction>();
				if (relatedFaction == a_faction && reactionToFaction->fightReaction >= RE::FIGHT_REACTION::kAlly) {
					return true;
				}
			}

			return false;
		});
	}

	bool HUDMarkerManager::IsPlayerOpponentOfFaction(const RE::TESFaction* a_faction) const
	{
		return player->VisitFactions([a_faction](RE::TESFaction* a_visitedFaction, std::int8_t a_rank) -> bool {
			if (a_visitedFaction == a_faction && a_rank < 0) {
				return true;
			}

			for (RE::GROUP_REACTION* reactionToFaction : a_visitedFaction->reactions) {
				auto relatedFaction = reactionToFaction->form->As<RE::TESFaction>();
				if (relatedFaction == a_faction && reactionToFaction->fightReaction == RE::FIGHT_REACTION::kEnemy) {
					return true;
				}
			}

			return false;
		});
	}

	std::string HUDMarkerManager::GetSideInQuest(RE::QUEST_DATA::Type a_questType) const
	{
		switch (a_questType) {
		case RE::QUEST_DATA::Type::kCivilWar:
			if (IsPlayerAllyOfFaction(sonsOfSkyrimFaction) ||
				IsPlayerAllyOfFaction(stormCloaksFaction) ||
				IsPlayerOpponentOfFaction(imperialLegionFaction)) {
				return "StormCloaks";
			} else {
				return "ImperialLegion";
			}
		case RE::QUEST_DATA::Type::kDLC01_Vampire:
			if (player->HasKeywordString("Vampire") ||
				IsPlayerAllyOfFaction(vampireFaction) ||
				IsPlayerOpponentOfFaction(dawnGuardFaction)) {
				return "Vampires";
			} else {
				return "Dawnguard";
			}
		}

		return {};
	}
}
