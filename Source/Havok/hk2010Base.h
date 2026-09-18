#pragma once
#include "Havok/hkSharedBase.h"

namespace hk2010_2_0
{
	// Local typedefs making it easier when writing header files using these types.

	using hkInt32         = hkShared::hkInt32;
	using hkReal          = hkShared::hkReal;
	using hkInt8          = hkShared::hkInt8;
	using hkUint32        = hkShared::hkUint32;
	using hkUint8         = hkShared::hkUint8;
	using hkUint16        = hkShared::hkUint16;
	using hkInt16         = hkShared::hkInt16;
	using hkUint64        = hkShared::hkUint64;
	using hkInt64         = hkShared::hkInt64;
	using hkUintReal      = hkShared::hkUintReal;
	using hkLong          = hkShared::hkLong;
	using hkHalf16        = hkShared::hkHalf16;
	using hk_size_t       = hkShared::hk_size_t;
	using hkBool          = hkShared::hkBool;

	using hkVector4       = hkShared::hkVector4;
	using hkQuaternion    = hkShared::hkQuaternion;
	using hkRotationf     = hkShared::hkRotationf;
	using hkMatrix3Impl   = hkShared::hkMatrix3Impl;
	using hkMatrix3f      = hkShared::hkMatrix3f;
	using hkQuaternionf   = hkShared::hkQuaternionf;
	using hkVector4f      = hkShared::hkVector4f;
	using hkTransform     = hkShared::hkTransform;
	using hkQsTransform   = hkShared::hkQsTransform;

	// the *ONE* exception
	using hkUlong = hkShared::hkUint32;
}