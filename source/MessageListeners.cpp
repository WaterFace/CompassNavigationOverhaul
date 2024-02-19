#include "Settings.h"

#include "IUI/API.h"

#include "Compass.h"
#include "QuestItemList.h"
#include "Test.h"

#include "utils/GFxLoggers.h"
#include "utils/Logger.h"

void InfinityUIMessageListener(SKSE::MessagingInterface::Message* a_msg);

void SKSEMessageListener(SKSE::MessagingInterface::Message* a_msg)
{
	// If all plugins have been loaded
	if (a_msg->type == SKSE::MessagingInterface::kPostLoad) {
		if (SKSE::GetMessagingInterface()->RegisterListener("InfinityUI", InfinityUIMessageListener)) {
			logger::info("Successfully registered for Infinity UI messages!");
		} else {
			logger::error("Infinity UI installation not detected. Please, go to ... to get it");
		}
	}
}

void InfinityUIMessageListener(SKSE::MessagingInterface::Message* a_msg)
{
	if (!a_msg || std::string_view(a_msg->sender) != "InfinityUI") {
		return;
	}

	if (auto message = IUI::API::TranslateAs<IUI::API::Message>(a_msg)) {
		std::string_view movieUrl = message->movie->GetMovieDef()->GetFileURL();

		if (movieUrl.find("HUDMenu") == std::string::npos) {
			return;
		}

		GFxMemberLogger<logger::level::debug> memberLogger;

		switch (a_msg->type) {
		case IUI::API::Message::Type::kStartLoadInstances:
			{
				logger::info("Started loading patches");
				break;
			}
		case IUI::API::Message::Type::kPreReplaceInstance:
			{
				if (auto preReplaceMessage = IUI::API::TranslateAs<IUI::API::PreReplaceInstanceMessage>(a_msg)) {
					std::string pathToOriginal = preReplaceMessage->originalInstance.ToString().c_str();

					if (pathToOriginal == extended::Compass::path) {
						extended::Compass::InitSingleton(preReplaceMessage->originalInstance);
						auto compass = extended::Compass::GetSingleton();

						logger::debug("");
						logger::debug("Before patching");

						memberLogger.LogMembersOf(*compass);

						RE::GPointF coord = compass->LocalToGlobal();
						logger::debug("{} is on ({}, {})", compass->ToString(), coord.x, coord.y);
					}
				}
				break;
			}
		case IUI::API::Message::Type::kPostPatchInstance:
			{
				if (auto postPatchMessage = IUI::API::TranslateAs<IUI::API::PostPatchInstanceMessage>(a_msg)) {
					std::string pathToNew = postPatchMessage->newInstance.ToString().c_str();

					if (pathToNew == extended::Compass::path) {
						// We initialised the CompassShoutMeterHolder singleton in the pre-replace step,
						// if not, there has been an error
						if (auto compass = extended::Compass::GetSingleton()) {
							compass->SetupMod(postPatchMessage->newInstance);
							compass->SetUnits(settings::display::useMetricUnits);

							logger::debug("");
							logger::debug("After patching");

							memberLogger.LogMembersOf(*compass);

							RE::GPointF coord = compass->LocalToGlobal();
							logger::debug("{} is on ({}, {})", compass->ToString(), coord.x, coord.y);
						} else {
							logger::error("Compass instance counterpart not ready for {}", extended::Compass::path);
						}
					} else if (pathToNew == QuestItemList::path) {
						QuestItemList::InitSingleton(postPatchMessage->newInstance);
						auto questItemList = QuestItemList::GetSingleton();

						logger::debug("");
						logger::debug("Post-patch members of");

						memberLogger.LogMembersOf(*questItemList);

						RE::GPointF coord = questItemList->LocalToGlobal();
						logger::debug("{} is on ({}, {})", questItemList->ToString(), coord.x, coord.y);
					}
				}
				break;
			}
		case IUI::API::Message::Type::kAbortPatchInstance:
			{
				if (auto abortPatchMessage = IUI::API::TranslateAs<IUI::API::AbortPatchInstanceMessage>(a_msg)) {
					std::string pathToOriginal = abortPatchMessage->originalValue.ToString().c_str();

					if (pathToOriginal == extended::Compass::path) {
						logger::error("Aborted replacement of {}", extended::Compass::path);
					}
				}
				break;
			}
		case IUI::API::Message::Type::kFinishLoadInstances:
			{
				auto finishLoadMessage = IUI::API::TranslateAs<IUI::API::FinishLoadInstancesMessage>(a_msg);
				logger::info("Finished loading patches");
				break;
			}
		case IUI::API::Message::Type::kPostInitExtensions:
			{
				if (auto postInitExtMessage = IUI::API::TranslateAs<IUI::API::PostInitExtensionsMessage>(a_msg)) {
					if (auto questItemList = QuestItemList::GetSingleton()) {
						questItemList->AddToHudElements();

						logger::debug("QuestItemList added to HUD elements");
					}

					logger::debug("Extensions initialization finished");
				}
				break;
			}
		default:
			break;
		}
	}
}
