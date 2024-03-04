#pragma once

#include "utils/GFxDisplayObject.h"

namespace extended
{
	class Compass : public GFxDisplayObject
	{
	public:
		static constexpr inline std::string_view path = "_level0.HUDMovieBaseInstance.CompassShoutMeterHolder.Compass";

		static void InitSingleton(const GFxDisplayObject& a_originalCompass)
		{
			if (!singleton) {
				static Compass singletonInstance{ a_originalCompass };
				singleton = &singletonInstance;
			}
		}

		static Compass* GetSingleton() { return singleton; }

		void SetupMod(const GFxDisplayObject& a_replaceCompass)
		{
			if (a_replaceCompass.HasMember("Compass")) {
				*static_cast<GFxDisplayObject*>(this) = a_replaceCompass;
				Invoke("Compass");
			}
		}

		void SetUnits(bool a_useMetric)
		{
			Invoke("SetUnits", a_useMetric);
		}

		void SetFocusedMarkerInfo(const std::string& a_target, float a_distance, float a_heightDifference, uint32_t a_markerIndex)
		{
			Invoke("SetFocusedMarkerInfo", a_target.c_str(), a_distance, a_heightDifference, a_markerIndex);
		}

		void FocusMarker(std::uint32_t a_markerIndex)
		{
			Invoke("FocusMarker", a_markerIndex);
		}

		void UnfocusMarker()
		{
			Invoke("UnfocusMarker");
		}

		void UpdateMarker()
		{
			Invoke("UpdateFocusedMarker");
		}

		void SetMarkers()
		{
			Invoke("SetMarkers");
		}

	private:
		Compass(const GFxDisplayObject& a_originalCompass) :
			GFxDisplayObject{ a_originalCompass }
		{}

		static inline Compass* singleton = nullptr;
	};
}
