#pragma once

namespace hkShared
{
	// Forward declarations as necessary
	class hkpEntity;
	struct hkVector4f;
	struct hkQuaternionf;
	struct hkMatrix3f;

	// Typedefs
	typedef unsigned long long hkUlong;       //1419D2548
	typedef int hkInt32;                      //1419D2278
	typedef float hkReal;                     //1419D1668
	typedef signed char hkInt8;               //1419D1F48
	typedef unsigned int hkUint32;            //1419D22E8
	typedef unsigned char hkUint8;            //1419D20F8
	typedef unsigned short hkUint16;          //1419D21F8
	typedef short hkInt16;                    //1419D2178
	typedef unsigned long long hkUint64;      //1419D24C8
	typedef long long hkInt64;                //1419D2448
	typedef unsigned int hkUintReal;          //1419D2768
	typedef long long hkLong;                 //1419D26E8
	typedef hkInt16 hkHalf16;                 //1419D2178
	typedef size_t hk_size_t;

	typedef hkVector4f hkVector4;             //1419D1888
	typedef hkQuaternionf hkQuaternion;
	typedef hkMatrix3f hkRotationf;
	typedef hkMatrix3f hkMatrix3Impl;

#define HK_FORCE_INLINE __forceinline

	// Utility types

	struct __declspec(align(16)) hkVector4f
	{
		float x{};
		float y{};
		float z{};
		float w{};
	};

	struct hkMatrix3f
	{
		hkVector4f col0;
		hkVector4f col1;
		hkVector4f col2;

		HK_FORCE_INLINE float operator() (int row, int col)
		{
			float* f = (float*)&getColumn(col);
			return f[row];
		}
		HK_FORCE_INLINE const float operator() (int row, int col) const
		{
			float* f = (float*)&getColumn(col);
			return f[row];
		}
		HK_FORCE_INLINE hkVector4f& getColumn(int x)
		{
			return (&col0)[x];
		}
		HK_FORCE_INLINE const hkVector4f& getColumn(int x) const
		{
			return (&col0)[x];
		}
	};

	struct __declspec(align(16)) hkQuaternionf
	{
		float x{};
		float y{};
		float z{};
		float w = 1;

		void set(const hkRotationf& rot)
		{
			const float diagonal = rot(0,0) + rot(1,1) + rot(2,2);
			constexpr float half = 0.5f;

			if (diagonal > 0)
			{
				float s = sqrtf(diagonal + 1.0f);
				float t = half / s;
				x = (rot(2, 1) - rot(1, 2)) * t;
				y = (rot(0, 2) - rot(2, 0)) * t;
				z = (rot(1, 0) - rot(0, 1)) * t;
				w = half * s;
				return;
			}

			constexpr int next[] = { 1, 2, 0 };
			int i = 0;

			if (rot(1, 1) > rot(0, 0))
				i = 1;

			if (rot(2, 2) > rot(i, i))
				i = 2;

			int j = next[i];
			int k = next[j];

			float s = sqrtf(rot(i, i) - (rot(j, j) + rot(k, k)) + 1.0f);
			float t = half / s;

			float* q = &x;

			q[i] = half * s;
			q[j] = (rot(j, i) + rot(i, j)) * t;
			q[k] = (rot(k, i) + rot(i, k)) * t;
			q[3] = (rot(k, j) - rot(j, k)) * t;
		}
	};


	struct hkTransform
	{
		hkRotationf rotation;
		hkVector4f translation;
	};

	struct hkQsTransform
	{
		hkVector4f translation;
		hkQuaternionf rotation;
		hkVector4f scale;
	};

	// single-byte bool regardless of compiler settings
	// TODO: This can probably just be "bool" type as we're targeting Gens2024's MSVC compiler anyway...
	class hkBool
	{
	public:

		HK_FORCE_INLINE hkBool() : m_bool(0)
		{
		}

		HK_FORCE_INLINE hkBool(bool b)
		{
			m_bool = static_cast<char>(b);
		}

		HK_FORCE_INLINE operator bool() const
		{
			return m_bool != 0;
		}

		HK_FORCE_INLINE hkBool& operator=(bool e)
		{
			m_bool = static_cast<char>(e);
			return *this;
		}

		HK_FORCE_INLINE hkBool operator==(bool e) const
		{
			return (m_bool != 0) == e;
		}

		HK_FORCE_INLINE hkBool operator!=(bool e) const
		{
			return (m_bool != 0) != e;
		}

	private:
		char m_bool;
	};

}