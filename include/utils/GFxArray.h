#pragma once

#include "RE/G/GFxMovieView.h"
#include "RE/G/GFxValue.h"

class GFxArray : public RE::GFxValue
{
public:
	static GFxArray GetFrom(RE::GFxMovieView* a_movieView, const std::string_view& a_pathToArray)
	{
		RE::GFxValue array;

		a_movieView->GetVariable(&array, a_pathToArray.data());

		assert(array.IsArray());

		return array;
	}

	GFxArray(RE::GFxMovieView* a_movieView)
	{
		a_movieView->CreateArray(this);

		assert(GetMovieView() == a_movieView);
		assert(IsArray());
	}

	GFxArray(const RE::GFxValue& a_value)
	{
		*static_cast<RE::GFxValue*>(this) = a_value;

		assert(IsArray());
	}

	RE::GFxMovieView* GetMovieView()
	{
		return **reinterpret_cast<RE::GFxMovieView***>(static_cast<RE::GFxValue*>(this));
	}

	RE::GFxValue GetElement(std::uint32_t a_index) const
	{
		RE::GFxValue value;
		RE::GFxValue::GetElement(a_index, &value);
		return value;
	}

	bool SetElement(std::uint32_t a_index, const RE::GFxValue& a_value)
	{
		return RE::GFxValue::SetElement(a_index, &a_value);
	}

	std::int32_t FindElement(const RE::GFxValue& a_value)
	{
		for (std::uint32_t i = 0; i < GetArraySize(); i++) {
			if (GetElement(i) == a_value) {
				return i;
			}
		}

		return -1;
	}
};