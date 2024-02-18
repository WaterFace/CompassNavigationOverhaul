#pragma once

#include "RE/E/ExtraMapMarker.h"
#include "RE/G/GFxValue.h"

namespace RE
{
	class HUDMarker
	{
	public:
		struct FrameOffsets
		{
			static FrameOffsets* GetSingleton()
			{
				REL::Relocation<FrameOffsets*> singleton{ RELOCATION_ID(519587, 406118) };
				return singleton.get();
			}

			std::uint32_t quest;				 // 00
			std::uint32_t questDoor;			 // 04
			std::uint32_t playerSet;			 // 08
			std::uint32_t enemy;				 // 0C
			std::uint32_t location;				 // 10
			std::uint32_t undiscoveredLocation;	 // 14
		};
		static_assert(sizeof(FrameOffsets) == 0x18);

		struct ScaleformData
		{
			GFxValue heading;  // 00 - angle relative to camera
			GFxValue alpha;	   // 18
			GFxValue type;	   // 30 - Goto and stop frame in flash
			GFxValue scale;	   // 48
		};
		static_assert(sizeof(ScaleformData) == 0x60);

		std::uint32_t GetLocationGotoFrame(const MapMarkerData& a_mapMarker) const
		{
			auto frameOffsets = FrameOffsets::GetSingleton();

			std::uint32_t locationFrameOffset = (a_mapMarker.flags & MapMarkerData::Flag::kCanTravelTo) ?
													frameOffsets->location :
													frameOffsets->undiscoveredLocation;

			return locationFrameOffset + a_mapMarker.type.underlying() - 1;
		}

		bool IsQuestFrame(std::uint32_t a_frame) const
		{
			auto frameOffsets = FrameOffsets::GetSingleton();

			return a_frame == frameOffsets->quest || a_frame == frameOffsets->questDoor;
		}
	};

	class HUDMarkerManager
	{
	public:
		static HUDMarkerManager* GetSingleton()
		{
			REL::Relocation<HUDMarkerManager*> singleton{ RELOCATION_ID(519611, 406154) };
			return singleton.get();
		}

		HUDMarker::ScaleformData scaleformMarkerData[49];  // 0000
		NiPoint3 position[48];							   // 1260
		BSTArray<RefHandle> locationRefs;				   // 14A0
		float sqRadiusToAddLocation;					   // 14B8
		std::uint32_t currentMarkerIndex;				   // 14BC
		std::uint32_t unk14C0;							   // 14C0 - Related to locations
	};
	static_assert(sizeof(HUDMarkerManager) == 0x14C8);
}