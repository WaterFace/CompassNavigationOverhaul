#pragma once

#include "utils/Trampoline.h"

#include "RE/B/BSCoreTypes.h"
#include "RE/C/Compass.h"
#include "RE/H/HUDMarkerManager.h"
#include "RE/N/NiPoint3.h"

namespace hooks
{
	class HUDMarkerManager
	{
		static constexpr REL::RelocationID UpdateQuestsId = RELOCATION_ID(50826, 51691);
		static constexpr REL::RelocationID UpdateLocationsId = RELOCATION_ID(50870, 51744);
		static constexpr REL::RelocationID AddMarkerId = RELOCATION_ID(50851, 51728);

	public:

		static inline REL::Relocation<bool (*)(const RE::HUDMarkerManager*, RE::TESQuest**,
			RE::BSTArray<RE::TESQuestTarget>*)> UpdateQuests{ UpdateQuestsId };

		static inline REL::Relocation<bool (*)(const RE::HUDMarkerManager*)> UpdateLocations{ UpdateLocationsId };

		static inline REL::Relocation<bool (*)(const RE::HUDMarkerManager*, RE::HUDMarker::ScaleformData*,
											   RE::NiPoint3*, const RE::RefHandle&, std::int32_t)> AddMarker{ AddMarkerId };
	};

	class HUDMenu
	{
		static constexpr REL::RelocationID ProcessMessageId = RELOCATION_ID(50718, 51612);

	public:

		static inline REL::Relocation<RE::UI_MESSAGE_RESULTS (*)(const RE::HUDMenu*, RE::UIMessage&)> ProcessMessage{ ProcessMessageId };
	};

	class Compass
	{
		static constexpr REL::RelocationID SetMarkersId = RELOCATION_ID(50775, 51670);
		static constexpr REL::RelocationID UpdateId = RELOCATION_ID(50773, 51668);

	public:

		static inline REL::Relocation<bool (*)(RE::Compass*)> SetMarkers{ SetMarkersId };
		static inline REL::Relocation<void (*)(RE::Compass*)> Update{ UpdateId };
	};

	bool UpdateQuests(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
							RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame,
		RE::TESQuestTarget* a_questTargets);

	RE::TESWorldSpace* AllowedToShowMapMarker(const RE::TESObjectREFR* a_marker);

	bool UpdateLocations(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
							   RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame);

	bool UpdateEnemies(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
							RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame);

	bool UpdatePlayerSetMarker(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
								RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame);

	bool SetCompassMarkers(RE::GFxValue::ObjectInterface* a_objectInterface, void* a_data, 
						   RE::GFxValue* a_result, const char* a_name, const RE::GFxValue* a_args, 
						   std::uint32_t a_numArgs, bool a_isDObj);

	static inline void Install()
	{
		// HUDMarkerManager::UpdateQuests (call to HUDMarkerManager::AddMarker(...))
		{
			static std::uintptr_t hookedAddress = HUDMarkerManager::UpdateQuests.address() + (REL::Module::IsSE() ? 0x114 : 0x180);

			struct HookCodeGenerator : Xbyak::CodeGenerator
			{
				HookCodeGenerator()
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					//There's not enough room for both of these, as rsp+0x30 is in use, but we can deduce the quest
					// from the quest target

					//mov(ptr[rsp + 0x28], REL::Module::IsSE() ? r14 : rdi);	// SE: r14, AE: rdi = TESQuest**
					mov(ptr[rsp + 0x28], rbx);	// TESQuestTarget*
					call(ptr[rip + hookLabel]);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&UpdateQuests));
					L(retnLabel), dq(hookedAddress + 5);
				}
			};

			utils::WriteBranchTrampoline<5>(hookedAddress, HookCodeGenerator());
		}

		// HUDMarkerManager::UpdateLocations (call to TESObjectREFR::GetWorldspace())
		{
			static std::uintptr_t hookedAddress1 = HUDMarkerManager::UpdateLocations.address() + (REL::Module::IsSE() ? 0x139 : 0x13C);
			static std::uintptr_t hookedAddress2 = HUDMarkerManager::UpdateLocations.address() + (REL::Module::IsSE() ? 0x21C : 0x24B);

			struct HookCodeGenerator : Xbyak::CodeGenerator
			{
				HookCodeGenerator(std::uintptr_t a_hookAddress)
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					call(ptr[rip + hookLabel]);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&AllowedToShowMapMarker));
					L(retnLabel), dq(a_hookAddress + 5);
				}
			};

			utils::WriteBranchTrampoline<5>(hookedAddress1, HookCodeGenerator(hookedAddress1));
			utils::WriteBranchTrampoline<5>(hookedAddress2, HookCodeGenerator(hookedAddress2));
		}

		// HUDMarkerManager::UpdateLocations call to HUDMarkerManager::AddMarker(...) for locations
		{
			static std::uintptr_t hookedAddress = HUDMarkerManager::UpdateLocations.address() + (REL::Module::IsSE() ? 0x450 : 0x473);

			struct HookCodeGenerator : Xbyak::CodeGenerator
			{
				HookCodeGenerator()
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					call(ptr[rip + hookLabel]);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&UpdateLocations));
					L(retnLabel), dq(hookedAddress + 5);
				}
			};

			utils::WriteBranchTrampoline<5>(hookedAddress, HookCodeGenerator());
		}

		// HUDMenu::ProcessMessage call to HUDMarkerManager::AddMarker(...) for enemies
		{
			static std::uintptr_t hookedAddress = HUDMenu::ProcessMessage.address() + (REL::Module::IsSE() ? 0x15AB : 0x1695);

			struct HookCodeGenerator : Xbyak::CodeGenerator
			{
				HookCodeGenerator()
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					call(ptr[rip + hookLabel]);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&UpdateEnemies));
					L(retnLabel), dq(hookedAddress + 5);
				}
			};

			// utils::WriteBranchTrampoline<5>(hookedAddress, HookCodeGenerator());
		}

		// Compass::SetMarkers call to HUDMarkerManager::AddMarker(...) for Player-set marker
		{
			static std::uintptr_t hookedAddress = Compass::SetMarkers.address() + 0x8D;

			struct HookCodeGenerator : Xbyak::CodeGenerator
			{
				HookCodeGenerator(std::uintptr_t a_hookAddress)
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					call(ptr[rip + hookLabel]);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&UpdatePlayerSetMarker));
					L(retnLabel), dq(a_hookAddress + 5);
				}
			};

			utils::WriteBranchTrampoline<5>(hookedAddress, HookCodeGenerator(hookedAddress));

			// In AE Compass::SetMarkers is inlined in Compass::Update, hook both
			if (REL::Module::IsAE()) 
			{
				static std::uintptr_t hookedAddressAE = Compass::Update.address() + 0xAE;

				utils::WriteBranchTrampoline<5>(hookedAddressAE, HookCodeGenerator(hookedAddressAE));
			}
		}

		// Compass::SetMarkers call to movie->Invoke("SetCompassMarkers", ...)
		{
			static std::uintptr_t hookedAddress = Compass::SetMarkers.address() + 0x10F;

			struct HookCodeGenerator : Xbyak::CodeGenerator
			{
				HookCodeGenerator(std::uintptr_t a_hookAddress)
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					call(ptr[rip + hookLabel]);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&SetCompassMarkers));
					L(retnLabel), dq(a_hookAddress + 5);
				}
			};

			utils::WriteBranchTrampoline<5>(hookedAddress, HookCodeGenerator(hookedAddress));

			// In AE Compass::SetMarkers is inlined in Compass::Update, hook both
			if (REL::Module::IsAE()) 
			{
				static std::uintptr_t hookedAddressAE = Compass::Update.address() + 0x128;

				utils::WriteBranchTrampoline<5>(hookedAddressAE, HookCodeGenerator(hookedAddressAE));
			}
		}
	}
}
