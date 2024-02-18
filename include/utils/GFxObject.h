#pragma once

#include "RE/G/GFxMovieView.h"
#include "RE/G/GFxValue.h"

class GFxObject : public RE::GFxValue
{
public:
	static GFxObject GetFrom(RE::GFxMovieView* a_movieView, const std::string_view& a_pathToObject)
	{
		RE::GFxValue object;

		a_movieView->GetVariable(&object, a_pathToObject.data());

		assert(object.IsObject());

		return object;
	}

	template <typename... Args>
	GFxObject(RE::GFxMovieView* a_movieView, Args&&... args)
	{
		std::array<RE::GFxValue, sizeof...(Args)> gfxArgs{ std::forward<Args>(args)... };

		a_movieView->CreateObject(this, nullptr, sizeof...(Args) ? &gfxArgs[0] : nullptr, sizeof...(Args));

		assert(GetMovieView() == a_movieView);
		assert(IsObject());
	}

	template <typename... Args>
	GFxObject(RE::GFxMovieView* a_movieView, const std::string_view& a_className, Args&&... args)
	{
		std::array<RE::GFxValue, sizeof...(Args)> gfxArgs{ std::forward<Args>(args)... };

		a_movieView->CreateObject(this, a_className.data(), sizeof...(Args) ? &gfxArgs[0] : nullptr, sizeof...(Args));

		assert(GetMovieView() == a_movieView);
		assert(IsObject());
	}

	GFxObject(const RE::GFxValue& a_value)
	{
		*static_cast<RE::GFxValue*>(this) = a_value;

		assert(IsObject());
	}

	RE::GFxMovieView* GetMovieView()
	{
		return **reinterpret_cast<RE::GFxMovieView***>(static_cast<RE::GFxValue*>(this));
	}

	RE::GFxValue GetMember(const std::string_view& a_memberName) const
	{
		RE::GFxValue value;
		RE::GFxValue::GetMember(a_memberName.data(), &value);
		return value;
	}

	bool SetMember(const std::string_view& a_memberName, const RE::GFxValue& a_value)
	{
		return RE::GFxValue::SetMember(a_memberName.data(), a_value);
	}

	template <typename... Args>
	RE::GFxValue Invoke(const std::string_view& a_functionName, Args&&... args)
	{
		RE::GFxValue result;
		Invoke(a_functionName, &result, args...);
		GetMovieView()->Advance(0.0F);
		return result;
	}

	template <typename... Args>
	bool Invoke(const std::string_view& a_functionName, RE::GFxValue* a_result, Args&&... args)
	{
		std::array<RE::GFxValue, sizeof...(Args)> gfxArgs{ std::forward<Args>(args)... };

		return RE::GFxValue::Invoke(a_functionName.data(), a_result,
			sizeof...(Args) ? &gfxArgs[0] : nullptr, sizeof...(Args));
	}
};