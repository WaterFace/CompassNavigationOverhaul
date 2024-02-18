#include "Settings.h"

#include "utils/INISettingCollection.h"

namespace settings
{
	using namespace utils;

	void Init(const std::string& a_iniFileName)
	{
		INISettingCollection* iniSettingCollection = INISettingCollection::GetSingleton();

		{
			using namespace debug;
			iniSettingCollection->AddSettings(
				MakeSetting("uLogLevel:Debug", static_cast<std::uint32_t>(logLevel)));
		}
		{
			using namespace display;
			iniSettingCollection->AddSettings(
				MakeSetting("bUseMetricUnits:Display", useMetricUnits),
				MakeSetting("bShowUndiscoveredLocationMarkers:Display", showUndiscoveredLocationMarkers),
				MakeSetting("bUndiscoveredMeansUnknownMarkers:Display", undiscoveredMeansUnknownMarkers),
				MakeSetting("bUndiscoveredMeansUnknownInfo:Display", undiscoveredMeansUnknownInfo),
				MakeSetting("bShowEnemyMarkers:Display", showEnemyMarkers),
				MakeSetting("bShowObjectiveAsTarget:Display", showObjectiveAsTarget),
				MakeSetting("bShowInteriorMarkers:Display", showInteriorMarkers),
				MakeSetting("fAngleToShowMarkerDetails:Display", angleToShowMarkerDetails),
				MakeSetting("fAngleToKeepMarkerDetailsShown:Display", angleToKeepMarkerDetailsShown));
		}
		{
			using namespace questlist;
			iniSettingCollection->AddSettings(
				MakeSetting("fPositionX:QuestList", positionX),
				MakeSetting("fPositionY:QuestList", positionY),
				MakeSetting("fMaxHeight:QuestList", maxHeight),
				MakeSetting("bShowInExteriors:QuestList", showInExteriors),
				MakeSetting("bShowInInteriors:QuestList", showInInteriors),
				MakeSetting("fWalkingDelayToShow:QuestList", walkingDelayToShow),
				MakeSetting("fJoggingDelayToShow:QuestList", joggingDelayToShow),
				MakeSetting("fSprintingDelayToShow:QuestList", sprintingDelayToShow),
				MakeSetting("bHideInCombat:QuestList", hideInCombat));
		}

		if (!iniSettingCollection->ReadFromFile(a_iniFileName)) {
			logger::warn("Could not read {}, falling back to default options", a_iniFileName);
		}

		{
			using namespace debug;
			logLevel = static_cast<logger::level>(iniSettingCollection->GetSetting<std::uint32_t>("uLogLevel:Debug"));
		}
		{
			using namespace display;
			showObjectiveAsTarget = iniSettingCollection->GetSetting<bool>("bShowObjectiveAsTarget:Display");
			showInteriorMarkers = iniSettingCollection->GetSetting<bool>("bShowInteriorMarkers:Display");
			useMetricUnits = iniSettingCollection->GetSetting<bool>("bUseMetricUnits:Display");
			showUndiscoveredLocationMarkers = iniSettingCollection->GetSetting<bool>("bShowUndiscoveredLocationMarkers:Display");
			undiscoveredMeansUnknownMarkers = iniSettingCollection->GetSetting<bool>("bUndiscoveredMeansUnknownMarkers:Display");
			undiscoveredMeansUnknownInfo = iniSettingCollection->GetSetting<bool>("bUndiscoveredMeansUnknownInfo:Display");
			showEnemyMarkers = iniSettingCollection->GetSetting<bool>("bShowEnemyMarkers:Display");
			angleToShowMarkerDetails = iniSettingCollection->GetSetting<float>("fAngleToShowMarkerDetails:Display");
			angleToKeepMarkerDetailsShown = iniSettingCollection->GetSetting<float>("fAngleToKeepMarkerDetailsShown:Display");
		}
		{
			using namespace questlist;
			positionX = iniSettingCollection->GetSetting<float>("fPositionX:QuestList");
			positionY = iniSettingCollection->GetSetting<float>("fPositionY:QuestList");
			maxHeight = iniSettingCollection->GetSetting<float>("fMaxHeight:QuestList");
			showInExteriors = iniSettingCollection->GetSetting<bool>("bShowInExteriors:QuestList");
			showInInteriors = iniSettingCollection->GetSetting<bool>("bShowInInteriors:QuestList");
			walkingDelayToShow = iniSettingCollection->GetSetting<float>("fWalkingDelayToShow:QuestList");
			joggingDelayToShow = iniSettingCollection->GetSetting<float>("fJoggingDelayToShow:QuestList");
			sprintingDelayToShow = iniSettingCollection->GetSetting<float>("fSprintingDelayToShow:QuestList");
			hideInCombat = iniSettingCollection->GetSetting<bool>("bHideInCombat:QuestList");
		}
	}
}