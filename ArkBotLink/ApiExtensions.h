#pragma once

#include "API/Ark/Ark.h"

namespace ArkExtensions
{
	struct Globals
	{
		static DataValue<int> GServerMajorVersion() { return { "Global.GServerMajorVersion" }; }
		static DataValue<int> GServerMinorVersion() { return { "Global.GServerMinorVersion" }; }
		static DataValue<UEngine*> GEngine() { return { "Global.GEngine" }; }
	};

	struct APrimalWorldSettings : AWorldSettings
	{

		bool& bOverrideLongitudeAndLatitudeField() { return *GetNativePointerField<bool*>(this, "APrimalWorldSettings.bOverrideLongitudeAndLatitude"); }
		float& LongitudeScaleField() { return *GetNativePointerField<float*>(this, "APrimalWorldSettings.LongitudeScale"); }
		float& LatitudeScaleField() { return *GetNativePointerField<float*>(this, "APrimalWorldSettings.LatitudeScale"); }
		float& LongitudeOriginField() { return *GetNativePointerField<float*>(this, "APrimalWorldSettings.LongitudeOrigin"); }
		float& LatitudeOriginField() { return *GetNativePointerField<float*>(this, "APrimalWorldSettings.LatitudeOrigin"); }
	};

	inline bool IsPointInside2dCircle(FVector point, float circleX, float circleY, float circleRadius)
	{
		long double x = point.X - circleX;
		long double y = point.Y - circleY;

		long double distancesq = x * x + y * y;
		return distancesq < circleRadius * circleRadius;
	}
}