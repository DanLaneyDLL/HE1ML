#pragma once
//#include "hk2022Base.h"
//#include "hk2010Base.h"

namespace hk2022
{
	class hkpShape;
	class hkpPhysicsData;
}
namespace hk2010_2_0
{
	class hkpShape;
}

namespace hkConvert
{
	void Init();

	static uint64_t pGenerateRigidBodies = 0x140BFCB80;
	static uint64_t pGetHavokClassFromResource = 0x14076C1A0;

	static std::unordered_map<const void*, void*> uniquePointerMap;
	static std::unordered_map<const void*, const char*> s_classNameMap;

	static hk2022::hkpPhysicsData* s_PhysicsData2022 = nullptr;

	hk2022::hkpShape* DeepCopyShape(const hk2010_2_0::hkpShape* pShape2010);

	template<typename T, typename... Args>
	static T* GetOrMakeDuplicatePtr(const void* sourcePtr, Args&&... args)
	{
		if (!sourcePtr)
			return nullptr;

		if (uniquePointerMap.contains(sourcePtr))
			return (T*)uniquePointerMap.at(sourcePtr);

		T* result = new T(std::forward<Args>(args)...);
		uniquePointerMap.emplace(sourcePtr, result);

		return result;
	}

	// Unused for now, but might be helpful if we ever run into
	// same-size structs that can be easily copied.
	template <typename T, typename A>
	static void StructCopy(T& dst, A& src)
	{
		static_assert(sizeof(T) == sizeof(A), "Structs must be of equal size to copy!");
		memcpy(&dst, &src, sizeof(T));
	}
}