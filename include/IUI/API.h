#pragma once

namespace IUI::API
{
	struct Message
	{
		enum Type : std::uint32_t
		{
			kStartLoad,
			kPreReplace,
			kPostPatch,
			kAbortPatch,
			kFinishLoad,
			kPostInitExtensions
		};

		RE::GFxMovieView* contextMovieView;
		std::string_view contextMovieUrl;
	};

	struct StartLoadMessage : Message
	{
		static constexpr inline Type type = Type::kStartLoad;
	};

	struct PreReplaceMessage : Message
	{
		static constexpr inline Type type = Type::kPreReplace;

		RE::GFxValue& originalDisplayObject;
	};

	struct PostPatchMessage : Message
	{
		static constexpr inline Type type = Type::kPostPatch;

		RE::GFxValue& newDisplayObject;
	};

	struct AbortPatchMessage : Message
	{
		static constexpr inline Type type = Type::kAbortPatch;

		RE::GFxValue& originalValue;
	};

	struct FinishLoadMessage : Message
	{
		static constexpr inline Type type = Type::kFinishLoad;

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