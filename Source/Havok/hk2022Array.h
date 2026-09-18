#pragma once
#include "hk2022Base.h"


#if 1
namespace hk2022
{
	static uint64_t pHkContainerHeapAllocator = 0x1419D01A0;
	static uint64_t pHkReserveMore = 0x140567F30;

	template <typename T>
	class hkArray
	{
	public:
		T* m_data;   //0x1419D9E10
		int m_size;   //0x1419D99E0
		//int m_capacityAndFlags = -2147483648;   //0x1419D99E0
		int m_capacityAndFlags;   //0x1419D99E0

		static inline FUNCTION_PTR(void, __fastcall, hkReserveMore, pHkReserveMore, hkArray* array, uint64_t alloc, uint64_t newSize);

		HK_FORCE_INLINE void resize(int newSize)
		{
			if ((m_capacityAndFlags & 0x7FFFFFFF) < newSize)
				hkReserveMore(this, pHkContainerHeapAllocator, newSize);

			m_size = newSize;
		}

		void emplace_back(const T& inData)
		{
			const int i = m_size;

			resize(m_size + 1);
			
			m_data[i] = inData;
		}

		HK_FORCE_INLINE void push_back(const T& inData)
		{
			emplace_back(inData);
		}

		template <typename A>
		void copy(const hk2010_2_0::hkArray<A>& in_CopyArray)
		{
			static_assert(sizeof(T) == sizeof(A), "Array types must be of equal size for a valid copy!");
			resize(in_CopyArray.m_size);
			memcpy(m_data, in_CopyArray.m_data.get(), in_CopyArray.m_size * sizeof(T));
		}

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
			return m_data;
		}

		iterator end()
		{
			return m_data + m_size;
		}

		const_iterator begin() const
		{
			return m_data;
		}

		const_iterator end() const
		{
			return m_data + m_size;
		}
	};
}
#else
namespace hk2022
{
	template <typename T>
	class hkArrayBase
	{
		//friend class hkTrackerArrayLayoutHandler;
		template<typename TYPE> friend class hkArraySpu;

	public:

		typedef hkArrayBase<T> ThisType;
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_ARRAY, ThisType);

		/// Creates a zero length array.
		HK_FORCE_INLINE hkArrayBase();

		/// Noncopying initialization from an existing external buffer.
		/// This does not copy the array but uses it in place until its capacity
		/// is exceeded at which point a reallocation occurs and the array behaves
		/// like a normal hkArray.
		/// The caller must ensure that the buffer is valid for the lifetime
		/// of this array and for deallocation of the buffer.
		HK_FORCE_INLINE hkArrayBase(T* buffer, int size, int capacity);

	protected:

		/// Array cannot be copied without explicit allocator.
		HK_FORCE_INLINE hkArrayBase(const hkArrayBase& a) {  }

		/// Array cannot be copied without explicit allocator.
		HK_FORCE_INLINE hkArrayBase& operator= (const hkArrayBase& a) {  return *this; }

	public:

		/// Destructs array members.
		HK_FORCE_INLINE ~hkArrayBase();

		/// Read/write access to the i'th element.
		HK_FORCE_INLINE T& operator[] (int i);

		/// Read only access to the i'th element.
		HK_FORCE_INLINE const T& operator[] (int i) const;

		/// Read/write access to the last element.
		HK_FORCE_INLINE T& back();

		/// Read only access to the last element.
		HK_FORCE_INLINE const T& back() const;

		/// Returns the size.
		HK_FORCE_INLINE int getSize() const;

		/// Returns the capacity.
		HK_FORCE_INLINE int getCapacity() const;

		/// Checks if the size is zero.
		HK_FORCE_INLINE hkBool isEmpty() const;

		/// Sets the size to zero.
		HK_FORCE_INLINE void clear();

		/// Sets the size to zero and deallocates storage.
		HK_FORCE_INLINE void _clearAndDeallocate(hkMemoryAllocator& alloc);

		/// Tries to reduce the capacity to avoid wasting storage. If shrinkExact is true the resulting capacity
		/// is size+numElementsLeft
		HK_FORCE_INLINE void _optimizeCapacity(hkMemoryAllocator& alloc, int numFreeElemsLeft, hkBool32 shrinkExact = false);

		/// Removes the element at the specified index. The last array element is used to replace the removed element, and the size is reduced by 1.
		/// This is very fast, but note that the order of elements is changed.
		HK_FORCE_INLINE void removeAt(int index);

		/// Removes the element at the specified index, copying elements down one slot as in the STL array.
		/// Slower than removeAt(), but the order is unchanged.
		HK_FORCE_INLINE void removeAtAndCopy(int index);

		/// Removes several elements at the specified index, copying elements down as in the STL array.
		HK_FORCE_INLINE void removeAtAndCopy(int index, int numToRemove);

		/// Returns the index of the first occurrence of t, or -1 if not found.
		HK_FORCE_INLINE int indexOf(const T& t, int start = 0, int end = -1) const;

		/// Returns index of the last occurrence of t, or -1 if not found.
		HK_FORCE_INLINE int lastIndexOf(const T& t) const;

		/// Removes the last element.
		HK_FORCE_INLINE void popBack(int numElemsToRemove = 1);

		/// Adds an element to the end.
		HK_FORCE_INLINE void _pushBack(hkMemoryAllocator& alloc, const T& e);

		/// Adds an element to the end. No check for resize.
		HK_FORCE_INLINE void pushBackUnchecked(const T& e);

		/// Ensures no reallocation occurs until at least size n.
		HK_FORCE_INLINE void _reserve(hkMemoryAllocator& alloc, int n);

		/// Ensures no reallocation occurs until size n.
		HK_FORCE_INLINE void _reserveExactly(hkMemoryAllocator& alloc, int n);

		/// Sets the size.
		/// If the array is expanded, new elements are uninitialized.
		HK_FORCE_INLINE void _setSize(hkMemoryAllocator& alloc, int size);

		/// Sets the size to n.
		/// If the array is expanded, new elements initialized with 'fill'.
		HK_FORCE_INLINE void _setSize(hkMemoryAllocator& alloc, int n, const T& fill);

		/// Sets the size assuming the capacity to be sufficient.
		/// If the array is expanded, new elements are uninitialized.
		HK_FORCE_INLINE void setSizeUnchecked(int size);

		/// Increments the size by 1 and returns a reference to the first element created.
		HK_FORCE_INLINE T& _expandOne(hkMemoryAllocator& alloc);

		/// Increments the size by n and returns a pointer to the first element created.
		HK_FORCE_INLINE T* _expandBy(hkMemoryAllocator& alloc, int n);

		/// Increments the size by n and returns a pointer to the first element created. No check for resize!
		HK_FORCE_INLINE T* expandByUnchecked(int n);

		/// Expands the array by numToInsert at the specified index.
		HK_FORCE_INLINE T* _expandAt(hkMemoryAllocator& alloc, int index, int numToInsert);

		/// Inserts the array a at index i.
		void _insertAt(hkMemoryAllocator& alloc, int i, const T* a, int numElems);

		/// Inserts t at index i.
		/// Elements from i to the end are copied up one place.
		void _insertAt(hkMemoryAllocator& alloc, int i, const T& t);

		/// Appends the array a.
		void _append(hkMemoryAllocator& alloc, const T* a, int numElems);

		/// Replaces elements [i,i+ndel) with the supplied array.
		/// This method avoids redundant copying associated with separate remove & insert steps.
		void _spliceInto(hkMemoryAllocator& alloc, int i, int ndel, const T* p, int numElems);


		/// Removes all occurrences of t, preserving the order of the remaining elements.
		HK_FORCE_INLINE void removeAllAndCopy(const T& t);

		/// Adds the element at the end of the array, or returns false if the array would have
		/// to be resized first.
		HK_FORCE_INLINE hkBool tryPushBack(const T& t);

		///
		typedef T* iterator;
		///
		typedef const T* const_iterator;

		/// Returns an STL-like iterator to the first element.
		HK_FORCE_INLINE iterator begin();
		/// Returns an STL-like iterator to the 'one past the last' element.
		HK_FORCE_INLINE iterator end();
		/// Returns an STL-like const iterator to the first element.
		HK_FORCE_INLINE const_iterator begin() const;
		/// Returns an STL-like const iterator to the 'one past the last' element.
		HK_FORCE_INLINE const_iterator end() const;

		static HK_FORCE_INLINE void HK_CALL copy(T* dst, const T* src, int n);

		/// Set the storage of the array to use the provided
		/// external memory allocation. Set size and capacity as provided
		/// by the user. Existing array data is forgotten without deallocating
		/// or destructing objects, use clearAndDeallocate if required.
		/// The array objects must be of POD type. In particular, destructors will
		/// be called on any objects remaining in the array when it is destructed
		/// The memory block is owned by the Array and will be deleted on destruction
		/// or resizing
		HK_FORCE_INLINE void setDataAutoFree(T* ptr, int size, int capacity);

		/// Set the storage of the array to use the provided
		/// external memory allocation. Set size and capacity as provided
		/// by the user. Existing array data is forgotten without deallocating
		/// or destructing objects, use clearAndDeallocate if required.
		/// The array objects must be of POD type. In particular, destructors will
		/// be called on any objects remaining in the array when it is destructed
		/// The memory block is not owned by the Array and will not be deleted on destruction
		/// or resizing
		HK_FORCE_INLINE void setDataUserFree(T* ptr, int size, int capacity);

		/// Advanced use only. Set the storage of the array, bypassing
		/// correctness checks. Size, capacity and flags are set as provided.
		/// Note that constructor/destructor semantics of non-POD types may not
		/// be respected.
		HK_FORCE_INLINE void _setDataUnchecked(T* ptr, int size, int capacityAndFlags);

		/// Get the capacity and the flags - advanced use
		HK_FORCE_INLINE int getCapacityAndFlags() const;

	protected:

		// Internal type checking
		HK_FORCE_INLINE void _setData(T* ptr, int size, int capacityAndFlags, hkTypeIsPod);

		HK_FORCE_INLINE hkArrayBase<T>& copyFromArray(hkMemoryAllocator&, const hkArrayBase<T>& src, hkTypeIsPod);
		HK_FORCE_INLINE hkArrayBase<T>& copyFromArray(hkMemoryAllocator&, const hkArrayBase<T>& src, hkTypeIsClass);

	public:

		// Public so that the serialization can access it.
		enum
		{
			CAPACITY_MASK = int(0x3FFFFFFF),
			FLAG_MASK = int(0xC0000000),
			DONT_DEALLOCATE_FLAG = int(0x80000000), // Indicates that the storage is not the array's to delete
			FORCE_SIGNED = -1
		};

	protected:

		friend class hkStatisticsCollector;

		T* m_data;
		int m_size;
		int m_capacityAndFlags; // highest 2 bits indicate any special considerations about the allocation for the array

	public:

		/// For serialization, we want to initialize the vtables
		/// in classes post data load, and NOT call the default constructor
		/// for the arrays (as the data has already been set).
		hkArrayBase(hkFinishLoadedObjectFlag f) {}
	};

	/// Array which uses the hkMemoryRouter::heap allocator.
	template <typename T, typename Allocator = hkContainerHeapAllocator>
	class hkArray : public hkArrayBase<T>
	{
	public:

		typedef Allocator AllocatorType;
		friend class hkArrayUtil;

		typedef hkArray<T, hkContainerTempAllocator> Temp;
		typedef hkArray<T, hkContainerDebugAllocator> Debug;
		typedef hkArray<T, hkContainerHeapAllocator> Heap;

		/// Creates a zero length array.
		HK_FORCE_INLINE hkArray() : hkArrayBase<T>() {}

		///
		HK_FORCE_INLINE ~hkArray();

		/// Creates an array of size n. All elements are uninitialized.
		explicit HK_FORCE_INLINE hkArray(int size);

		/// Creates an array of n elements initialized to 'fill'.
		HK_FORCE_INLINE hkArray(int size, const T& fill);

		/// Noncopying initialization from an existing external buffer.
		/// This does not copy the array but uses it in place until its capacity
		/// is exceeded at which point a reallocation occurs and the array behaves
		/// like a normal hkArray.
		/// The caller must ensure that the buffer is valid for the lifetime
		/// of this array and for deallocation of the buffer.
		HK_FORCE_INLINE hkArray(T* buffer, int size, int capacity) : hkArrayBase<T>(buffer, size, capacity) {}

		hkArray(hkFinishLoadedObjectFlag f) : hkArrayBase<T>(f) {}

	protected:

		/// Not publicly accessible, too easy to call accidentally.
		HK_FORCE_INLINE hkArray(const hkArray& a);

	public:

		/// Copy another array
		HK_FORCE_INLINE hkArray& operator= (const hkArrayBase<T>& a);
		HK_FORCE_INLINE hkArray& operator= (const hkArray& a);

		/// Swaps this array's internal storage with 'a'.
		void swap(hkArray& a);

		HK_FORCE_INLINE void clearAndDeallocate() { hkArrayBase<T>::_clearAndDeallocate(AllocatorType().get(this)); }
		HK_FORCE_INLINE void optimizeCapacity(int numFreeElemsLeft, hkBool32 shrinkExact = false) { hkArrayBase<T>::_optimizeCapacity(AllocatorType().get(this), numFreeElemsLeft, shrinkExact); }
		HK_FORCE_INLINE void pushBack(const T& e) { hkArrayBase<T>::_pushBack(AllocatorType().get(this), e); }
		HK_FORCE_INLINE void reserve(int size) { hkArrayBase<T>::_reserve(AllocatorType().get(this), size); }
		HK_FORCE_INLINE void reserveExactly(int size) { hkArrayBase<T>::_reserveExactly(AllocatorType().get(this), size); }
		HK_FORCE_INLINE void setSize(int size) { hkArrayBase<T>::_setSize(AllocatorType().get(this), size); }
		HK_FORCE_INLINE void setSize(int size, const T& fill) { hkArrayBase<T>::_setSize(AllocatorType().get(this), size, fill); }
		HK_FORCE_INLINE T& expandOne() { return hkArrayBase<T>::_expandOne(AllocatorType().get(this)); }
		HK_FORCE_INLINE T* expandBy(int n) { return hkArrayBase<T>::_expandBy(AllocatorType().get(this), n); }
		HK_FORCE_INLINE T* expandAt(int i, int n) { return hkArrayBase<T>::_expandAt(AllocatorType().get(this), i, n); }
		HK_FORCE_INLINE void insertAt(int i, const T& e) { hkArrayBase<T>::_insertAt(AllocatorType().get(this), i, e); }
		HK_FORCE_INLINE void insertAt(int i, const T* a, int numElems) { hkArrayBase<T>::_insertAt(AllocatorType().get(this), i, a, numElems); }
		HK_FORCE_INLINE void append(const T* a, int numElems) { hkArrayBase<T>::_append(AllocatorType().get(this), a, numElems); }
		HK_FORCE_INLINE void spliceInto(int i, int ndel, const T* p, int numElems) { return hkArrayBase<T>::_spliceInto(AllocatorType().get(this), i, ndel, p, numElems); }
	};



	template <typename T>
	HK_FORCE_INLINE T& hkArrayBase<T>::operator[] (int i)
	{
		return m_data[i];
	}

	template <typename T>
	HK_FORCE_INLINE const T& hkArrayBase<T>::operator[] (int i) const
	{
		return m_data[i];
	}

	template <typename T>
	HK_FORCE_INLINE T& hkArrayBase<T>::back()
	{
		return m_data[m_size - 1];
	}

	template <typename T>
	HK_FORCE_INLINE const T& hkArrayBase<T>::back() const
	{
		return m_data[m_size - 1];
	}

	template <typename T>
	HK_FORCE_INLINE int hkArrayBase<T>::getSize() const
	{
		return m_size;
	}

	template <typename T>
	HK_FORCE_INLINE int hkArrayBase<T>::getCapacity() const
	{
		return (m_capacityAndFlags & static_cast<int>(CAPACITY_MASK));
	}

	template <typename T>
	HK_FORCE_INLINE int hkArrayBase<T>::getCapacityAndFlags() const
	{
		return m_capacityAndFlags;
	}

	template <typename T>
	HK_FORCE_INLINE hkBool hkArrayBase<T>::isEmpty() const
	{
		return m_size == 0;
	}

	template <typename T>
	HK_FORCE_INLINE hkResult hkArrayBase<T>::_reserveExactly(hkMemoryAllocator& alloc, int n)
	{
		if (getCapacity() < n)
		{
			return hkArrayUtil::_reserve(alloc, this, n, sizeof(T));
		}

		return HK_SUCCESS;
	}

	template <typename T>
	HK_FORCE_INLINE hkResult hkArrayBase<T>::_reserve(hkMemoryAllocator& alloc, int n)
	{
		const int capacity = getCapacity();
		if (capacity < n)
		{
			int cap2 = 2 * capacity;
			int newSize = (n < cap2) ? cap2 : n;
			return hkArrayUtil::_reserve(alloc, this, newSize, sizeof(T));
		}

		return HK_SUCCESS;
	}

	template <typename T>
	HK_FORCE_INLINE void hkArrayBase<T>::clear()
	{
		hkArrayUtil::destruct(m_data, m_size, typename hkTrait::IsPodType<T>::type());
		m_size = 0;
	}



	template <typename T>
	HK_FORCE_INLINE void HK_CALL hkArrayBase<T>::copy(T* dst, const T* src, int n)
	{
		for (int i = 0; i < n; ++i)
		{
			dst[i] = src[i];
		}
	}



	template <typename T>
	HK_FORCE_INLINE hkArrayBase<T>::hkArrayBase()
		: m_data(HK_NULL),
		m_size(0),
		m_capacityAndFlags(DONT_DEALLOCATE_FLAG)
	{
	}

	template <typename T>
	HK_FORCE_INLINE hkArrayBase<T>& hkArrayBase<T>::copyFromArray(hkMemoryAllocator& alloc, const hkArrayBase<T>& a, hkTrait::TypeIsPod)
	{
		if (getCapacity() < a.getSize())
		{
			if ((m_capacityAndFlags & DONT_DEALLOCATE_FLAG) == 0)
			{
				alloc._bufFree<T>(m_data, getCapacity());
			}
			int n = a.getSize();
			m_data = alloc._bufAlloc<T>(n);
			m_capacityAndFlags = n;
		}
		m_size = a.getSize();
		copy(m_data, a.m_data, m_size);
		return *this;
	}

	template <typename T>
	HK_FORCE_INLINE hkArrayBase<T>& hkArrayBase<T>::copyFromArray(hkMemoryAllocator& alloc, const hkArrayBase<T>& a, hkTrait::TypeIsClass)
	{
		int oldSize = m_size;
		int newSize = a.getSize();
		int copiedSize = newSize > oldSize ? oldSize : newSize;

		_reserve(alloc, newSize); // ensure space
		hkArrayUtil::destruct(m_data + newSize, oldSize - newSize, hkTrait::TypeIsClass()); // destruct items past the size of a, if any
		copy(m_data, a.m_data, copiedSize); // copy objects into the 'live' part of this array
		hkArrayUtil::constructWithArray(m_data + copiedSize, newSize - copiedSize, a.m_data + copiedSize, hkTrait::TypeIsClass()); // and construct the rest
		m_size = newSize;
		return *this;
	}

	template <typename T>
	HK_FORCE_INLINE hkArrayBase<T>::hkArrayBase(T* ptr, int size, int capacity)
		: m_data(ptr),
		m_size(size),
		m_capacityAndFlags(capacity | DONT_DEALLOCATE_FLAG)
	{
		HK_ASSERT2(0x23483be5, size >= 0 && capacity >= 0, "Array size and capacity must be non-negative.");
		HK_ASSERT2(0x4234325a, size <= capacity, "The size must be consistent with the capacity as memory will not be allocated in constructor. Use 'setSize' to allocate memory if inplace capacity too small.");
	}

	template <typename T>
	HK_FORCE_INLINE hkArrayBase<T>::~hkArrayBase()
	{
		HK_ASSERT2(0x1129f768, hkTrait::IsPodType<T>::result || m_size == 0, "Non-POD array elements not destructed");
		HK_ASSERT2(0x1129f769, m_capacityAndFlags & DONT_DEALLOCATE_FLAG, "Array memory not freed");
	}

	template <typename T>
	HK_FORCE_INLINE void hkArrayBase<T>::_clearAndDeallocate(hkMemoryAllocator& alloc)
	{
		clear();
		if ((m_capacityAndFlags & DONT_DEALLOCATE_FLAG) == 0)
		{
			const int SIZE_ELEM = hkSizeOfTypeOrVoid<T>::val;
			int numBytes = getCapacity() * SIZE_ELEM;
			void* dataPtr = const_cast<typename hkTrait::RemoveConst<T>::type*>(m_data);
#if defined(HK_PLATFORM_PPU)
			if (m_capacityAndFlags & ALLOCATED_FROM_SPU)
			{
				numBytes = hkMemoryRouterSpuUtil::getAllocatedSize(dataPtr, numBytes);
			}
#endif
			alloc.bufFree(dataPtr, numBytes);
		}
		m_data = HK_NULL;
		m_capacityAndFlags = DONT_DEALLOCATE_FLAG;
	}

#define HK_COMPUTE_OPTIMIZED_CAPACITY( size, numFreeElemsLeft, shrinkExact ) (shrinkExact) ? (size + numFreeElemsLeft) : hkNextPowerOf2(size + numFreeElemsLeft)

	template <typename T>
	HK_FORCE_INLINE void hkArrayBase<T>::_optimizeCapacity(hkMemoryAllocator& alloc, int numFreeElemsLeft, hkBool32 shrinkExact)
	{
		int totalCapacity = (shrinkExact) ? (m_size + numFreeElemsLeft) : hkNextPowerOf2(m_size + numFreeElemsLeft);
		if (totalCapacity < getCapacity())
		{
			hkArrayUtil::_reduce(alloc, this, sizeof(T), HK_NULL, totalCapacity);
		}
	}

	template <typename T>
	HK_FORCE_INLINE void hkArrayBase<T>::removeAt(int index)
	{

		hkArrayUtil::destruct(&m_data[index], 1, typename hkTrait::IsPodType<T>::type());
		m_size--;
		if (m_size != index)
		{
			hkMemUtil::memCpyOneAligned<sizeof(T), HK_ALIGN_OF(T)>(m_data + index, m_data + m_size);
		}
	}

	template <typename T>
	HK_FORCE_INLINE void hkArrayBase<T>::removeAtAndCopy(int index)
	{

		hkArrayUtil::destruct(&m_data[index], 1, typename hkTrait::IsPodType<T>::type());
		m_size--;
		hkMemUtil::memCpy<HK_ALIGN_OF(T)>(m_data + index, m_data + index + 1, (m_size - index) * sizeof(T));
	}

	template <typename T>
	HK_FORCE_INLINE void hkArrayBase<T>::removeAtAndCopy(int index, int numToRemove)
	{

		hkArrayUtil::destruct(m_data + index, numToRemove, typename hkTrait::IsPodType<T>::type());
		m_size -= numToRemove;
		hkMemUtil::memCpy<HK_ALIGN_OF(T)>(m_data + index, m_data + index + numToRemove, (m_size - index) * sizeof(T));
	}

	template <typename T>
	HK_FORCE_INLINE int hkArrayBase<T>::indexOf(const T& t, int startIdx, int endIdx) const
	{
		if (endIdx < 0)
		{
			endIdx = m_size;
		}
		for (int i = startIdx; i < endIdx; ++i)
		{
			if (m_data[i] == t)
			{
				return i;
			}
		}
		return -1;
	}

	template <typename T>
	HK_FORCE_INLINE int hkArrayBase<T>::lastIndexOf(const T& t) const
	{
		for (int i = m_size - 1; i >= 0; --i)
		{
			if (m_data[i] == t)
			{
				return i;
			}
		}
		return -1;
	}

	template <typename T>
	HK_FORCE_INLINE void hkArrayBase<T>::popBack(int numToRemove)
	{
		hkArrayUtil::destruct(m_data + m_size - numToRemove, numToRemove, typename hkTrait::IsPodType<T>::type());
		m_size -= numToRemove;
	}

	template <typename T>
	HK_FORCE_INLINE void hkArrayBase<T>::_pushBack(hkMemoryAllocator& alloc, const T& t)
	{
		if (m_size == getCapacity())
		{
			HK_ASSERT2(0x76e453e4, !((&t >= m_data) && (&t < (m_data + m_size))), "hkArrayBase::pushBack can't push back element of same array during resize");
			hkArrayUtil::_reserveMore(alloc, this, sizeof(T));
		}
		hkArrayUtil::constructWithCopy<T>(m_data + m_size, 1, t, typename hkTrait::IsPodType<T>::type());
		m_size++;
	}

	template <typename T>
	HK_FORCE_INLINE void hkArrayBase<T>::pushBackUnchecked(const T& t)
	{
		hkArrayUtil::constructWithCopy<T>(m_data + m_size, 1, t, typename hkTrait::IsPodType<T>::type());
		m_size++;
	}


	template <typename T>
	HK_FORCE_INLINE hkBool hkArrayBase<T>::tryPushBack(const T& t)
	{
		if (m_size < getCapacity())
		{
			hkArrayUtil::constructWithCopy(m_data + m_size, 1, t, typename hkTrait::IsPodType<T>::type());
			m_size++;
			return true;
		}
		else
		{
			return false;
		}
	}

	template <typename T>
	HK_FORCE_INLINE void hkArrayBase<T>::_setSize(hkMemoryAllocator& alloc, int n)
	{
		_reserve(alloc, n);
		hkArrayUtil::destruct(m_data + n, m_size - n, typename hkTrait::IsPodType<T>::type());
		hkArrayUtil::construct(m_data + m_size, n - m_size, typename hkTrait::IsPodType<T>::type());
		m_size = n;
	}

	template <typename T>
	HK_FORCE_INLINE void hkArrayBase<T>::_setSize(hkMemoryAllocator& alloc, int n, const T& fill)
	{
		_reserve(alloc, n);
		hkArrayUtil::destruct(m_data + n, m_size - n, typename hkTrait::IsPodType<T>::type());
		hkArrayUtil::constructWithCopy(m_data + m_size, n - m_size, fill, typename hkTrait::IsPodType<T>::type());
		m_size = n;
	}

	template <typename T>
	HK_FORCE_INLINE hkResult hkArrayBase<T>::_trySetSize(hkMemoryAllocator& alloc, int n)
	{
		hkResult res = _reserve(alloc, n);
		if (res == HK_SUCCESS)
		{
			hkArrayUtil::destruct(m_data + n, m_size - n, typename hkTrait::IsPodType<T>::type());
			hkArrayUtil::construct(m_data + m_size, n - m_size, typename hkTrait::IsPodType<T>::type());
			m_size = n;
		}
		return res;
	}


	template <typename T>
	HK_FORCE_INLINE void hkArrayBase<T>::setSizeUnchecked(int n)
	{
		hkArrayUtil::destruct(m_data + n, m_size - n, typename hkTrait::IsPodType<T>::type());
		hkArrayUtil::construct(m_data + m_size, n - m_size, typename hkTrait::IsPodType<T>::type());
		m_size = n;
	}

	template <typename T>
	HK_FORCE_INLINE T* hkArrayBase<T>::_expandBy(hkMemoryAllocator& alloc, int n)
	{
		int oldsize = m_size;
		_reserve(alloc, m_size + n);
		hkArrayUtil::construct(m_data + m_size, n, typename hkTrait::IsPodType<T>::type());
		m_size += n;
		return m_data + oldsize;
	}

	template <typename T>
	HK_FORCE_INLINE T& hkArrayBase<T>::_expandOne(hkMemoryAllocator& alloc)
	{
		if (m_size == getCapacity())
		{
			hkArrayUtil::_reserveMore(alloc, this, sizeof(T));
		}
		hkArrayUtil::construct(m_data + m_size, 1, typename hkTrait::IsPodType<T>::type());
		return m_data[m_size++];
	}

	template <typename T>
	HK_FORCE_INLINE T* hkArrayBase<T>::expandByUnchecked(int n)
	{
		int oldsize = m_size;
		m_size = oldsize + n;
		hkArrayUtil::construct(m_data + oldsize, m_size - oldsize, typename hkTrait::IsPodType<T>::type());
		return m_data + oldsize;
	}

	template <typename T>
	void hkArrayBase<T>::_spliceInto(hkMemoryAllocator& alloc, int index, int numdel, const T* p, int numtoinsert)
	{
		const int newsize = numtoinsert + m_size - numdel;
		const int numtomove = m_size - index - numdel;
		if (newsize > getCapacity())
		{
			// note double copy from [i:end] not a problem in practice
			_reserve(alloc, newsize);
		}
		hkArrayUtil::destruct(m_data + index, numdel, typename hkTrait::IsPodType<T>::type());
		hkMemUtil::memMove(m_data + index + numtoinsert, m_data + index + numdel, numtomove * sizeof(T));
		hkArrayUtil::constructWithArray(m_data + index, numtoinsert, p, typename hkTrait::IsPodType<T>::type());
		m_size = newsize;
	}

	template <typename T>
	void hkArrayBase<T>::_insertAt(hkMemoryAllocator& alloc, int index, const T* p, int numtoinsert)
	{
		_spliceInto(alloc, index, 0, p, numtoinsert);
	}

	template <typename T>
	void hkArrayBase<T>::_append(hkMemoryAllocator& alloc, const T* a, int numtoinsert)
	{
		const int newsize = m_size + numtoinsert;
		if (newsize > getCapacity())
		{
			_reserve(alloc, newsize);
		}
		hkArrayUtil::constructWithArray(m_data + m_size, numtoinsert, a, typename hkTrait::IsPodType<T>::type());
		m_size = newsize;
	}

	template <typename T>
	HK_FORCE_INLINE T* hkArrayBase<T>::_expandAt(hkMemoryAllocator& alloc, int index, int numtoinsert)
	{

		const int newsize = numtoinsert + m_size;
		const int numtomove = m_size - index;
		if (newsize > getCapacity())
		{
			// note double copy from [i:end] not a problem in practice
			_reserve(alloc, newsize);
		}
		hkMemUtil::memMove(m_data + index + numtoinsert, m_data + index, numtomove * sizeof(T));
		hkArrayUtil::construct(m_data + index, numtoinsert, typename hkTrait::IsPodType<T>::type());
		m_size = newsize;
		return m_data + index;
	}

	template <typename T>
	void hkArrayBase<T>::_insertAt(hkMemoryAllocator& alloc, int i, const T& t)
	{
		_insertAt(alloc, i, &t, 1);
	}

	template <typename T>
	HK_FORCE_INLINE void hkArrayBase<T>::removeAllAndCopy(const T& t)
	{
		int destination = 0;
		// Do no copying until we've found a t.
		while ((destination < m_size) && (m_data[destination] != t))
		{
			++destination;
		}
		// If we have found a t, start copying.
		for (int source = destination; destination < m_size; ++source)
		{
			if (m_data[source] != t)
			{
				hkMemUtil::memCpyOneAligned<sizeof(T), HK_ALIGN_OF(T)>(m_data + destination, m_data + source);
				++destination;
			}
			else
			{
				hkArrayUtil::destruct(m_data + source, 1, typename hkTrait::IsPodType<T>::type());
				--m_size;
			}
		}
	}


	template <typename T>
	typename hkArrayBase<T>::iterator hkArrayBase<T>::begin()
	{
		return m_data;
	}

	template <typename T>
	typename hkArrayBase<T>::iterator hkArrayBase<T>::end()
	{
		return m_data + m_size;
	}

	template <typename T>
	typename hkArrayBase<T>::const_iterator hkArrayBase<T>::begin() const
	{
		return m_data;
	}

	template <typename T>
	typename hkArrayBase<T>::const_iterator hkArrayBase<T>::end() const
	{
		return m_data + m_size;
	}

	template <typename T>
	void hkArrayBase<T>::_setDataUnchecked(T* ptr, int size, int capacityAndFlags)
	{
		m_data = ptr;
		m_size = size;
		m_capacityAndFlags = capacityAndFlags;
	}


	template <typename T>
	void hkArrayBase<T>::_setData(T* ptr, int size, int capacityAndFlags, hkTrait::TypeIsPod)
	{
		hkArrayBase<T>::_setDataUnchecked(ptr, size, capacityAndFlags);
	}

	// template <typename T>
	// void hkArrayBase<T>::_setData(T *ptr, int size, int capacityAndFlags, hkTypeIsClass)
	// {
	// 	HK_ASSERT2(0x6335ef93, 0, "setOwnedData can only be called on POD types");
	// }

	template <typename T>
	void hkArrayBase<T>::setDataAutoFree(T* ptr, int size, int capacity)
	{
		hkArrayBase<T>::_setData(ptr, size, capacity, typename hkTrait::IsPodType<T>::type());
	}

	template <typename T>
	void hkArrayBase<T>::setDataUserFree(T* ptr, int size, int capacity)
	{
		hkArrayBase<T>::_setData(ptr, size, capacity | DONT_DEALLOCATE_FLAG, typename hkTrait::IsPodType<typename hkTrait::RemoveConst<T>::type>::type());
	}

	//
	// hkArray
	//

	template <typename T, typename Allocator>
	HK_FORCE_INLINE hkArray<T, Allocator>::~hkArray()
	{
		clearAndDeallocate();
	}

	template <typename T, typename Allocator>
	HK_FORCE_INLINE hkArray<T, Allocator>::hkArray(int n)
		: hkArrayBase<T>()
	{
		hkMemoryAllocator& a = Allocator().get(this);
		const int size = n;
		T* p = n ? a._bufAlloc<T>(n) : HK_NULL;
		int cap = n ? n : hkArrayBase<T>::DONT_DEALLOCATE_FLAG;
		hkArrayBase<T>::_setDataUnchecked(p, size, cap);
		hkArrayUtil::construct(p, size, typename hkTrait::IsPodType<T>::type());
	}

	template <typename T, typename Allocator>
	HK_FORCE_INLINE hkArray<T, Allocator>::hkArray(int n, const T& t)
		: hkArrayBase<T>()
	{
		hkMemoryAllocator& a = Allocator().get(this);
		const int size = n;
		T* p = n ? a._bufAlloc<T>(n) : HK_NULL;
		int cap = n ? n : hkArrayBase<T>::DONT_DEALLOCATE_FLAG;
		hkArrayBase<T>::_setDataUnchecked(p, size, cap);
		hkArrayUtil::constructWithCopy(p, size, t, typename hkTrait::IsPodType<T>::type());
	}

	template <typename T, typename Allocator>
	HK_FORCE_INLINE hkArray<T, Allocator>::hkArray(const hkArray<T, Allocator>& arr)
		: hkArrayBase<T>()
	{
		int n = arr.getSize();
		const int size = n;
		hkMemoryAllocator& a = Allocator().get(this);
		T* p = n ? a._bufAlloc<T>(n) : HK_NULL;
		int cap = n ? n : hkArrayBase<T>::DONT_DEALLOCATE_FLAG;
		hkArrayBase<T>::_setDataUnchecked(p, size, cap);
		hkArrayUtil::constructWithArray(p, size, arr.m_data, typename hkTrait::IsPodType<T>::type());
	}

	template <typename T, typename Allocator>
	HK_FORCE_INLINE hkArray<T, Allocator>& hkArray<T, Allocator>::operator= (const hkArrayBase<T>& a)
	{
		hkArrayBase<T>::copyFromArray(Allocator().get(this), a, typename hkTrait::IsPodType<T>::type());
		return *this;
	}

	template <typename T, typename Allocator>
	HK_FORCE_INLINE hkArray<T, Allocator>& hkArray<T, Allocator>::operator= (const hkArray<T, Allocator>& a)
	{
		hkArrayBase<T>::copyFromArray(Allocator().get(this), a, typename hkTrait::IsPodType<T>::type());
		return *this;
	}

	template <typename T, typename Allocator>
	void hkArray<T, Allocator>::swap(hkArray<T, Allocator>& a)
	{

		T* d = hkArrayBase<T>::m_data; // swap data
		hkArrayBase<T>::m_data = a.m_data;
		a.m_data = d;
		int s = hkArrayBase<T>::m_size; // swap size
		hkArrayBase<T>::m_size = a.m_size;
		a.m_size = s;
		int c = hkArrayBase<T>::m_capacityAndFlags; // swap cap
		hkArrayBase<T>::m_capacityAndFlags = a.m_capacityAndFlags;
		a.m_capacityAndFlags = c;
	}

	//
	// Inplace array
	//

	template <typename T, unsigned N, typename Allocator>
	HK_FORCE_INLINE hkInplaceArray<T, N, Allocator>::hkInplaceArray(int size)
		: hkArray<T, Allocator>(m_storage, size, N)
	{
	}

	template <typename T, unsigned N, typename Allocator>
	HK_FORCE_INLINE hkInplaceArray<T, N, Allocator>::hkInplaceArray(const hkInplaceArray<T, N, Allocator>& a)
		: hkArray<T, Allocator>(m_storage, 0, N)
	{
		*this = a;
	}

	template <typename T, unsigned N, typename Allocator>
	HK_FORCE_INLINE void hkInplaceArray<T, N, Allocator>::optimizeCapacity(int numFreeElemsLeft, hkBool32 shrinkExact)
	{
		if ((this->m_capacityAndFlags & hkArray<T, Allocator>::DONT_DEALLOCATE_FLAG) == 0)
		{
			int totalCapacity = HK_COMPUTE_OPTIMIZED_CAPACITY(this->m_size, numFreeElemsLeft, shrinkExact);
			if (totalCapacity < int(N))
			{
				// reducing to a capacity < N, we can copy back to local storage
				hkArrayUtil::_reduce(typename hkArray<T, Allocator>::AllocatorType().get(this), this, sizeof(T), (char*)(&m_storage[0]), N);
			}
			else if (totalCapacity < this->getCapacity())
			{
				hkArrayUtil::_reduce(typename hkArray<T, Allocator>::AllocatorType().get(this), this, sizeof(T), HK_NULL, totalCapacity);
			}
		}
	}

	template <typename T, unsigned N, typename Allocator>
	hkArray<T, Allocator>& hkInplaceArray<T, N, Allocator>::operator= (const hkArrayBase<T>& a)
	{
		return hkArray<T>::operator=(a);
	}

	template <typename T, unsigned N, typename Allocator>
	hkArray<T, Allocator>& hkInplaceArray<T, N, Allocator>::operator= (const hkInplaceArray<T, N, Allocator>& a)
	{
		return hkArray<T, Allocator>::operator=(a);
	}

	template <typename T, unsigned N, typename Allocator>
	hkBool hkInplaceArray<T, N, Allocator>::wasReallocated() const
	{
		return this->m_data != m_storage;
	}

	template <typename T, unsigned N, typename Allocator>
	int hkInplaceArray<T, N, Allocator>::stillInplaceUsingMask() const
	{
		return hkArray<T, Allocator>::m_capacityAndFlags& hkArrayBase<T>::DONT_DEALLOCATE_FLAG;
	}

	//
	// Inplace array 16
	//

	template <typename T, unsigned N>
	hkArray<T>& hkInplaceArrayAligned16<T, N>::operator= (const hkArrayBase<T>& a)
	{
		return hkArray<T>::operator=(a);
	}

	template <typename T, unsigned N>
	hkBool hkInplaceArrayAligned16<T, N>::wasReallocated() const
	{
		return uint64_t(this->m_data) != uint64_t(m_storage);
	}

	template <typename T, unsigned N>
	hkArray<T>& hkInplaceArrayAligned16<T, N>::operator= (const hkInplaceArrayAligned16<T, N>& a)
	{
		return hkArray<T>::operator=(a);
	}

	template <typename T, unsigned N>
	HK_FORCE_INLINE hkInplaceArrayAligned16<T, N>::hkInplaceArrayAligned16(int size)
		: hkArray<T>((T*)&m_storage[0], size, N)
	{
	}

	template <typename T, unsigned N>
	int hkInplaceArrayAligned16<T, N>::stillInplaceUsingMask() const
	{
		return hkArray<T>::m_capacityAndFlags & hkArrayBase<T>::DONT_DEALLOCATE_FLAG;
	}
#undef HK_COMPUTE_OPTIMIZED_CAPACITY

}
#endif