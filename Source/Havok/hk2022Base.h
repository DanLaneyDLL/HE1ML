#pragma once
#include "Havok/hkSharedBase.h"

namespace hk2022
{
	// Local typedefs making it easier when writing header files using these types.

	using hkUlong         = hkShared::hkUlong;
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

	// Allocation utilities for interfacing with the game.

	typedef void* (*tHkAlloc)(size_t, const size_t& nbytes, size_t);
	typedef void(*tHkDealloc)(size_t, void* This, size_t);

	// TODO: Sigscan these!
	static tHkAlloc&   HkAlloc   = *reinterpret_cast<tHkAlloc*>(0x141B2F8D8);
	static tHkDealloc& HkDealloc = *reinterpret_cast<tHkDealloc*>(0x141B2F8E0);
	
}
