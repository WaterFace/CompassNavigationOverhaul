#pragma once

#include "GFxObject.h"

class GFxDisplayObject : public GFxObject
{
public:
	GFxDisplayObject(const RE::GFxValue& a_value) :
		GFxObject{ a_value }
	{
		assert(a_value.IsDisplayObject());
	}

	GFxDisplayObject CreateEmptyMovieClip(const std::string_view& a_name, std::int32_t a_depth)
	{
		RE::GFxValue mc;
		RE::GFxValue::CreateEmptyMovieClip(&mc, a_name.data(), a_depth);
		return mc;
	}

	GFxDisplayObject CreateEmptyMovieClip(const std::string_view& a_name)
	{
		return CreateEmptyMovieClip(a_name, GetNextHighestDepth());
	}

	GFxDisplayObject AttachMovie(const std::string_view& a_className, const std::string_view& a_name, std::int32_t a_depth)
	{
		RE::GFxValue mc;
		RE::GFxValue::AttachMovie(&mc, a_className.data(), a_name.data(), a_depth);
		return mc;
	}

	GFxDisplayObject AttachMovie(const std::string_view& a_className, const std::string_view& a_name)
	{
		return AttachMovie(a_className, a_name, GetNextHighestDepth());
	}

	std::int32_t GetNextHighestDepth()
	{
		return static_cast<std::int32_t>(Invoke("getNextHighestDepth").GetNumber());
	}

	void SwapDepths(std::int32_t a_depth)
	{
		Invoke("swapDepths", a_depth);
	}

	void LoadMovie(const std::string_view& a_swfRelativePath)
	{
		Invoke("loadMovie", a_swfRelativePath.data());
	}

	void UnloadMovie()
	{
		Invoke("unloadMovie");
	}

	void RemoveMovieClip()
	{
		Invoke("removeMovieClip");
	}

	RE::GPointF LocalToGlobal()
	{
		GFxObject pt(GetMovieView());

		assert(pt.IsObject());

		RE::GFxValue x = 0.0;
		pt.SetMember("x", x);

		RE::GFxValue y = 0.0;
		pt.SetMember("y", y);

		Invoke("localToGlobal", pt);

		x = pt.GetMember("x");
		y = pt.GetMember("y");

		return { static_cast<float>(x.GetNumber()), static_cast<float>(y.GetNumber()) };
	}
};