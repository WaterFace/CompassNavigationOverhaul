#include "FocusedMarker.h"

#include "RE/H/HUDMarkerManager.h"

FocusedMarker::QuestData::QuestData(std::uint32_t a_gfxIndex, std::uint32_t a_gfxGotoFrame, RE::TESObjectREFR* a_marker,
	const RE::TESQuest* a_quest) :
	Data{ a_gfxIndex, a_gfxGotoFrame },
	quest{ a_quest }, isInSameLocation{ gfxGotoFrame == RE::HUDMarker::FrameOffsets::GetSingleton()->quest }
{
	// A quest marker can reference to a character or a location
	switch (a_marker->GetFormType()) {
	case RE::FormType::Reference:
		{
			if (auto teleportDoor = a_marker->As<RE::TESObjectREFR>()) {
				// If it is a teleport door, we can get the door at the other side
				if (auto teleportLinkedDoor = teleportDoor->extraList.GetTeleportLinkedDoor().get()) {
					// First, try interior cell
					if (RE::TESObjectCELL* cell = teleportLinkedDoor->GetParentCell()) {
						locationName = cell->GetName();
					}
					// Exterior cell
					else if (RE::TESWorldSpace* worldSpace = teleportLinkedDoor->GetWorldspace()) {
						locationName = worldSpace->GetName();
					}
				}
			}
			break;
		}
	case RE::FormType::ActorCharacter:
		{
			if (auto character = a_marker->As<RE::Character>()) {
				characterName = FocusedMarker::EnemyData::GetDisplayName(character);
			}
			break;
		}
	default:
		{
			break;
		}
	}
}