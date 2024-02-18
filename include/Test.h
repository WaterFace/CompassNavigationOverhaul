#pragma once

class Test : GFxDisplayObject
{
public:
	static constexpr inline std::string_view path = "_level0.Test";

	static void InitSingleton(const GFxDisplayObject& a_compassShoutMeterHolder)
	{
		if (!singleton) {
			static Test singletonInstance{ a_compassShoutMeterHolder };
			singleton = &singletonInstance;
		}
	}

	static Test* GetSingleton() { return singleton; }

	GFxDisplayObject textField0 = GetMember("TextField0");
	GFxDisplayObject textField1 = GetMember("TextField1");
	GFxDisplayObject textField2 = GetMember("TextField2");

private:
	Test(const GFxDisplayObject& a_test) :
		GFxDisplayObject{ a_test }
	{}

	static inline Test* singleton = nullptr;
};