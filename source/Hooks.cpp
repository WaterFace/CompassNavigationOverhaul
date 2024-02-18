#include "Hooks.h"

#include "Settings.h"

#include "HUDMarkerManager.h"

namespace hooks
{
	bool UpdateQuests(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame,
		RE::TESQuestTarget* a_questTarget)
	{
		//
		// The game loops through active quest targets, and calls `AddMarker` for each one.
		// If multiple targets correspond to the same marker, `AddMarker` returns the
		// previously-created marker (via `a_refHandle`), so we can iteratively
		// build the structure containing all the targets, objectives, etc. corresponding
		// to the marker.
		//

		if (HUDMarkerManager::AddMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerGotoFrame)) {
			RE::TESObjectREFR* marker = RE::TESObjectREFR::LookupByHandle(a_refHandle).get();

			RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
			RE::TESQuest* quest;
			RE::BGSInstancedQuestObjective objective;
			RE::TESQuestTarget* target;
			for (int i = 0; i < player->GetPlayerRuntimeData().objectives.size(); i++) {
				for (int j = 0; j < player->GetPlayerRuntimeData().objectives[i].objective->numTargets; j++) {
					if (a_questTarget->unk00 == (uint64_t)player->GetPlayerRuntimeData().objectives[i].objective->targets[j]) {
						quest = player->GetPlayerRuntimeData().objectives[i].objective->ownerQuest;
						objective = player->GetPlayerRuntimeData().objectives[i];
						target = (RE::TESQuestTarget*)a_questTarget->unk00;
						break;
					}
				}
			}

			extended::HUDMarkerManager::GetSingleton()->ProcessQuestMarker(quest, objective, target, marker, a_markerGotoFrame);

			return true;
		}

		return false;
	}

	RE::TESWorldSpace* AllowedToShowMapMarker(const RE::TESObjectREFR* a_marker)
	{
		RE::TESWorldSpace* markerWorldspace = a_marker->GetWorldspace();

		if (settings::display::showInteriorMarkers) {
			auto player = RE::PlayerCharacter::GetSingleton();

			RE::TESWorldSpace* playerWorldspace = player->GetWorldspace();

			if (playerWorldspace && markerWorldspace && playerWorldspace != markerWorldspace) {
				if (!playerWorldspace->parentWorld && markerWorldspace->parentWorld == playerWorldspace) {
					return playerWorldspace;
				}
			}
		}

		return markerWorldspace;
	}

	bool UpdateLocations(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame)
	{
		RE::TESObjectREFR* marker = RE::TESObjectREFR::LookupByHandle(a_refHandle).get();
		RE::TESWorldSpace* markerWorldspace = marker->GetWorldspace();

		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		RE::TESWorldSpace* playerWorldspace = player->GetWorldspace();

		RE::NiPoint3 markerRealPos = util::GetRealPosition(marker, markerWorldspace);
		RE::NiPoint3 playerRealPos = util::GetRealPosition(player, playerWorldspace);

		float sqDistanceToMarker = playerRealPos.GetSquaredDistance(markerRealPos);

		if (sqDistanceToMarker < RE::HUDMarkerManager::GetSingleton()->sqRadiusToAddLocation) {
			auto mapMarker = marker->extraList.GetByType<RE::ExtraMapMarker>();

			// Unvisited markers keep being shown in any case
			if (settings::display::showUndiscoveredLocationMarkers || mapMarker->mapData->flags.all(RE::MapMarkerData::Flag::kVisible)) {
				if (HUDMarkerManager::AddMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerGotoFrame)) {
					extended::HUDMarkerManager::GetSingleton()->ProcessLocationMarker(mapMarker, marker, a_markerGotoFrame);

					return true;
				}
			}
		}

		return false;
	}

	bool UpdateEnemies(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame)
	{
		if (settings::display::showEnemyMarkers) {
			if (HUDMarkerManager::AddMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerGotoFrame)) {
				RE::TESObjectREFR* marker = RE::TESObjectREFR::LookupByHandle(a_refHandle).get();

				extended::HUDMarkerManager::GetSingleton()->ProcessEnemyMarker(marker->As<RE::Character>(), a_markerGotoFrame);

				return true;
			}
		}

		return false;
	}

	bool UpdatePlayerSetMarker(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame)
	{
		if (HUDMarkerManager::AddMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerGotoFrame)) {
			RE::TESObjectREFR* marker = RE::TESObjectREFR::LookupByHandle(a_refHandle).get();

			extended::HUDMarkerManager::GetSingleton()->ProcessPlayerSetMarker(marker, a_markerGotoFrame);

			return true;
		}

		return false;
	}

	bool SetCompassMarkers(RE::GFxValue::ObjectInterface* a_objectInterface, void* a_data,
		RE::GFxValue* a_result, const char* a_name, const RE::GFxValue* a_args,
		std::uint32_t a_numArgs, bool a_isDObj)
	{
		if (a_objectInterface->Invoke(a_data, a_result, a_name, a_args, a_numArgs, a_isDObj)) {
			extended::HUDMarkerManager::GetSingleton()->SetMarkersExtraInfo();

			return true;
		}

		return false;
	}
}
