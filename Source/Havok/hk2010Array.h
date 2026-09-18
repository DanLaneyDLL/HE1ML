#pragma once
#include "hk2010Base.h"

// NOTE: This is a read-only type.
// We can iterate through it, but we should NOT modify any data directly.
namespace hk2010_2_0
{
	template <typename T>
	class hkArray
	{
	public:
		hl::off32<T> m_data;
		int m_size;
		int m_capacityAndFlags;

		// We don't want to naively copy arrays when iterating.
		hkArray(const hkArray&) = delete;
		hkArray& operator=(const hkArray&) = delete;

		// Allow moving if you need to populate the container
		hkArray(hkArray&&) = default;
		hkArray& operator=(hkArray&&) = default;

		HK_FORCE_INLINE T& operator[] (int i)
		{
			return m_data[i];
		}

		HK_FORCE_INLINE const T& operator[] (int i) const
		{
			return m_data[i];
		}

		typedef T* iterator;
		typedef const T* const_iterator;

		iterator begin()
		{
			return m_data.get();
		}

		iterator end()
		{
			return m_data.get() + m_size;
		}

		const_iterator begin() const
		{
			return m_data.get();
		}

		const_iterator end() const
		{
			return m_data.get() + m_size;
		}
	};
}