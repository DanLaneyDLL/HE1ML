#pragma once
//#include "hk2022Base.h"
//#include "hk2010Base.h"

namespace hk2022
{
	class hkpShape;
}
namespace hk2010_2_0
{
	class hkpShape;
}

namespace hkConvert
{
	void Init();

	static hk2022::hkpShape* DeepCopyShape(const hk2010_2_0::hkpShape* pShape2010);

	static std::unordered_map<const void*, void*> uniquePointerMap;
	static std::unordered_map<const void*, const char*> s_classNameMap;

	template<typename T, typename... Args>
	static T* MakeUniquePtr(const void* sourcePtr, Args&&... args)
	{
		if (!sourcePtr)
			return nullptr;

		if (uniquePointerMap.contains(sourcePtr))
			return (T*)uniquePointerMap.at(sourcePtr);

		T* result = new T(std::forward<Args>(args)...);
		uniquePointerMap.emplace(sourcePtr, result);

		return result;
	}

	template <typename T, typename A>
	static void PrimitiveCopy(T& dst, A& src)
	{
		static_assert(sizeof(T) == sizeof(A), "Types must be of equal size to copy!");
		memcpy(&dst, &src, sizeof(T));
	}
}