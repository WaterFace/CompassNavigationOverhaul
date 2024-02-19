#pragma once

namespace RE
{
	class GFxMovie;
	class GFxMovieDef;
	class GFxSpriteDef;
	class GFxValue;
	class IMenu;
}

namespace IUI::API
{
	struct Message
	{
		enum Type : std::uint32_t
		{
			kStartLoadInstances,
			kPreReplaceInstance,
			kPostPatchInstance,
			kAbortPatchInstance,
			kFinishLoadInstances,
			kPostInitExtensions
		};

		RE::IMenu* menu;
		RE::GFxMovie* movie;
	};

	struct StartLoadInstancesMessage : Message
	{
		static constexpr inline Type type = Type::kStartLoadInstances;
	};

	struct PreReplaceInstanceMessage : Message
	{
		static constexpr inline Type type = Type::kPreReplaceInstance;

		RE::GFxValue& originalInstance;
	};

	struct PostPatchInstanceMessage : Message
	{
		static constexpr inline Type type = Type::kPostPatchInstance;

		RE::GFxValue& newInstance;
		RE::GFxMovieDef* newInstanceMovieDef;
		RE::GFxSpriteDef* newInstanceSpriteDef;
	};

	struct AbortPatchInstanceMessage : Message
	{
		static constexpr inline Type type = Type::kAbortPatchInstance;

		RE::GFxValue& originalValue;
	};

	struct FinishLoadInstancesMessage : Message
	{
		static constexpr inline Type type = Type::kFinishLoadInstances;

		int loadedCount;
	};

	struct PostInitExtensionsMessage : Message
	{
		static constexpr inline Type type = Type::kPostInitExtensions;
	};

	template <typename T>
	concept valid_message = std::is_base_of_v<Message, T>;

	template <typename MessageT>
	requires valid_message<MessageT> inline const MessageT* TranslateAs(SKSE::MessagingInterface::Message* a_msg)
	{
		if constexpr (std::is_same_v<Message, MessageT>) {
			return static_cast<Message*>(a_msg->data);
		} else {
			if (a_msg->type == MessageT::type && a_msg->dataLen == sizeof(MessageT)) {
				return static_cast<MessageT*>(a_msg->data);
			}

			return nullptr;
		}
	}
}

/* API INSTRUCTIONS */
//
//
//	1.	After calling `SKSE::Init' in `SKSEPlugin_Load', register an SKSE messages listener function. In this function, register a
//		Infinity UI messages listener after that plugin is loaded (upon receiving a post load message from SKSE).
//		If Infinity UI is not detected, registering the listener will fail on game launch.
//
//		In `SKSEPlugin_Load':
//
//		...
//			if (!SKSE::GetMessagingInterface()->RegisterListener("SKSE", SKSEMessageListener))
//			{
//				// ... failure handling
//			}
//		...
//
//		Also define the SKSE message listener function elsewhere:
//
//		void SKSEMessageListener(SKSE::MessagingInterface::Message* a_msg)
//		{
//			// If all plugins have been loaded
//			if (a_msg->type == SKSE::MessagingInterface::kPostLoad) {
//				if (SKSE::GetMessagingInterface()->RegisterListener("InfinityUI", InfinityUIMessageListener)) {
//					// ... success handling
//				} else {
//					// ... failure handling
//				}
//			}
//		}
//
//
//	2.	Define the listener function for payload interpreter messages. The received `SKSE::MessagingInterface::Message'
//		will contain a `IUI::API::Message' in its "data" field. Depending on the SKSE message type, use `IUI::API::TranslateAs<>()'
//		to get the content of the message.
//
//		void InfinityUIMessageListener(SKSE::MessagingInterface::Message* a_msg)
//		{
//			if (!a_msg || std::string_view(a_msg->sender) != "InfinityUI") {
//				return;
//			}
//
//			// ... You can remove some of the cases in the switch below if you don't depend on them
//			switch (a_msg->type)
//			{
//			case IUI::API::Message::Type::kStartLoadInstances:
//				if (auto startLoadMsg = IUI::API::TranslateAs<IUI::API::StartLoadInstancesMessage>(a_msg)) {
//					// ... Infinity UI starts loading patches for a menu
//				}
//				break;
//			case IUI::API::Message::Type::kPreReplaceInstance:
//				if (auto preReplaceMsg = IUI::API::TranslateAs<IUI::API::PreReplaceInstanceMessage>(a_msg)) {
//					// ... The message contains data about the original instance to be retrieved before being replaced
//				}
//				break;
//			case IUI::API::Message::Type::kPostPatchInstance:
//				if (auto postPatchMsg = IUI::API::TranslateAs<IUI::API::PostPatchInstanceMessage>(a_msg)) {
//					// ... The message contains data about the instance that is replacing the old one
//				}
//				break;
//			case IUI::API::Message::Type::kAbortPatchInstance:
//				if (auto abortPatchMsg = IUI::API::TranslateAs<IUI::API::AbortPatchInstanceMessage>(a_msg)) {
//					// ... Infinity UI signals an abort message for you to take further actions.
//					// ... This happens when any element in the hierarchy to reach the instance to be replaced
//					// ... is not a movieclip.
//				}
//				break;
//			case IUI::API::Message::Type::kFinishLoadInstances:
//				if (auto finishLoadMsg = IUI::API::TranslateAs<IUI::API::FinishLoadInstancesMessage>(a_msg)) {
//					// ... Infinity UI finishes loading patches for a menu
//				}
//				break;
// 			case IUI::API::Message::Type::kPostInitExtensions:
//				if (auto postInitExtMsg = IUI::API::TranslateAs<IUI::API::PostInitExtensionsMessage>(a_msg)) {
//					// ... Some menus init extensions after loading the SWF to finish their setup.
//					// ... You may want to make some additional configurations before the menu starts running.
//				}
//				break;
//			}
//		}