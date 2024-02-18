#pragma once

#include "RE/H/HUDMarkerManager.h"

#include "Settings.h"

#include "Compass.h"
#include "FocusedMarker.h"
#include "QuestItemList.h"

namespace extended
{
	class HUDMarkerManager
	{
	public:
		static HUDMarkerManager* GetSingleton()
		{
			static HUDMarkerManager singleton;

			return &singleton;
		}

		void ProcessQuestMarker(RE::TESQuest* a_quest, RE::BGSInstancedQuestObjective& a_objective, RE::TESQuestTarget* a_target,
			RE::TESObjectREFR* a_marker, std::uint32_t a_markerGotoFrame);

		void ProcessLocationMarker(RE::ExtraMapMarker* a_mapMarker, RE::TESObjectREFR* a_marker,
			std::uint32_t a_markerGotoFrame);

		void ProcessEnemyMarker(RE::Character* a_enemy, std::uint32_t a_markerGotoFrame);

		void ProcessPlayerSetMarker(RE::TESObjectREFR* a_marker, std::uint32_t a_markerGotoFrame);

		void SetMarkersExtraInfo();

	private:
		std::shared_ptr<FocusedMarker> GetFacedMarkerUpdated(const RE::TESObjectREFR* a_marker, float a_angleToPlayerCamera)
		{
			std::shared_ptr<FocusedMarker> facedMarker;

			if (facedMarkers.contains(a_marker)) {
				facedMarker = facedMarkers.at(a_marker);
				facedMarker->UpdateGeometry(a_angleToPlayerCamera);
			} else {
				facedMarker = std::make_shared<FocusedMarker>(a_marker, a_angleToPlayerCamera);
				facedMarkers.emplace(a_marker, facedMarker);
			}

			return facedMarker;
		}

		bool IsFocusedMarker(const RE::TESObjectREFR* a_marker) const { return focusedMarker && a_marker == focusedMarker->ref; }

		std::shared_ptr<FocusedMarker>
			GetMostCenteredMarkerOf(const std::unordered_map<const RE::TESObjectREFR*, std::shared_ptr<FocusedMarker>>& a_facedMarkers);

		bool IsSameFocusedMarker(std::shared_ptr<FocusedMarker> a_nextFocusedMarker) const;

		float GetAngleBetween(const RE::PlayerCamera* a_playerCamera, const RE::TESObjectREFR* a_marker) const;

		bool IsPlayerAllyOfFaction(const RE::TESFaction* a_faction) const;

		bool IsPlayerOpponentOfFaction(const RE::TESFaction* a_faction) const;

		std::string GetSideInQuest(RE::QUEST_DATA::Type a_questType) const;

		Compass* compass = Compass::GetSingleton();
		QuestItemList* questItemList = QuestItemList::GetSingleton();

		float facingAngle = settings::display::angleToShowMarkerDetails;
		float keepFocusedAngle = settings::display::angleToKeepMarkerDetailsShown;

		std::unordered_map<const RE::TESObjectREFR*, std::shared_ptr<FocusedMarker>> facedMarkers;
		std::shared_ptr<FocusedMarker> focusedMarker;
		float timeFocusingMarker = 0.0F;

		RE::HUDMarkerManager* const hudMarkerManager = RE::HUDMarkerManager::GetSingleton();
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		RE::PlayerCamera* playerCamera = RE::PlayerCamera::GetSingleton();
		RE::BSTimer* timeManager = RE::BSTimer::GetTimeManager();

		// Factions to lookup
		// Reference: Creation Kit -> Skyrim.esm, Dawnguard.esm
		const RE::TESFaction* const imperialLegionFaction = RE::TESForm::LookupByID(0x0002BF9A)->As<RE::TESFaction>();
		const RE::TESFaction* const stormCloaksFaction = RE::TESForm::LookupByID(0x00028849)->As<RE::TESFaction>();
		const RE::TESFaction* const sonsOfSkyrimFaction = RE::TESForm::LookupByID(0x0002BF9B)->As<RE::TESFaction>();
		const RE::TESFaction* const dawnGuardFaction = RE::TESForm::LookupByID(0x02014217)->As<RE::TESFaction>();
		const RE::TESFaction* const vampireFaction = RE::TESForm::LookupByID(0x02003376)->As<RE::TESFaction>();
	};
}