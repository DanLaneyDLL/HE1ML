#pragma once

#include "hk2022Base.h"
#include "hk2022Array.h"

namespace hk2010_2_0
{
	class hkpConvexTransformShapeBase;
}

// Types dumped from Sonic Generations 2024 reflection data.
namespace hk2022
{
	// Irrelevant typedefs
#if 0
	typedef hkQsTransformf hkQsTransform;			//1419D1BC8
	typedef hkUint32 hkColor::Argb;			//1419D15E8
	typedef hkMatrix4f hkMatrix4;			//1419D1CD8
	typedef hkMatrix4Impl<float> hkMatrix4f;			//1419D1DE8
	typedef hkTransformf<float> hkTransform;			//1419D1A78
	typedef hkRotationImpl<float> hkRotationf;			//1419D1728
	typedef hkMatrix3Impl<float> hkMatrix3f;			//1419D1488
	typedef hkMatrix3Impl<double> hkMatrix3d;			//1419D14F8
	typedef hkRotationImpl<double> hkRotationd;			//1419D1918
	typedef hkMatrix3f hkMatrix3;			//1419D1C48
	typedef hkMatrix4Impl<double> hkMatrix4d;			//1419D1E68
	typedef hkRotationf hkRotation;			//1419D1D58
	typedef hkQuaternionf<float> hkQuaternion;			//1419D1A18
	typedef hkSimdFloat32 hkSimdReal;			//1419D17A8
	typedef hkVector2f hkVector2;			//1419D2858
	typedef hkFlags hkReflect::DeclFlags;			//1419D1998
	typedef hkColorf<float> hkColorfAlpha;			//1419D1B68
#endif

	// Sigscannable offsets.
	// TODO: Put ALL of these in a dedicated header and make sigs for them.
	static uint64_t pHkPhysicsSystemCtor = 0x14C27A270;
	static uint64_t pHkPhysicsDataCtor   = 0x14C63F6C0;
	static uint64_t pHkpRigidBodyCtor    = 0x1406A52F0;

	static uint64_t pHkpStorageExtendedMeshShapeCtor = 0x140662A90;
	static uint64_t pHkpConvexTransformShapeBaseCtor = 0x14063C050;
	static uint64_t pHkpConvexVerticesShapeCtor      = 0x140656DF0;

	static uint64_t pHkpMoppBvTreeShapeCtor  = 0x14064D0D0;
	static uint64_t pHkpListShapeCtor        = 0x140661200;
	static uint64_t pHkpBoxShapeCtor         = 0x14B96AAE0;

	// Constructors are inlined in latest version, use "create" fn
	static uint64_t pHkpConvexTranslateShapeCreate = 0x14B9D9CE0; 
	static uint64_t pHkpConvexTransformShapeCreate = 0x14063FB50;
	static uint64_t pHkpConvexVerticesConnectivityCreate = 0x14B9DEA80;

	// Forward declarations
	class hkpEntity;

#define MAKE_DEEP_COPY_FUNC(x) \
	static x* DeepCopy(const hk2010_2_0::hkpShape* in_pShape2010)

	// Actual classes

	class hkBaseObject
	{
	public:
		virtual ~hkBaseObject() {}
	};

	class hkReferencedObject : public hkBaseObject
	{
	public:
		hkUlong sizeAndFlags {};   //0x1419D2548
		hkUlong refCount {};   //0x1419D2548

		__forceinline void* __cdecl operator new(hk_size_t nbytes)
		{
			return HkAlloc(0,nbytes,0);
		}

		__forceinline void __cdecl operator delete(void* p)
		{
			HkDealloc(0,p,0);
		}

		__forceinline void* __cdecl operator new(hk_size_t, void* p) { return p; }
		__forceinline void __cdecl operator delete(void*, void*)
		{
		}

		void addReference()
		{
			FUNCTION_PTR(void, __fastcall, fn, 0x0140562600, hkReferencedObject * This);
			fn(this);
		}

		void removeReference()
		{
			FUNCTION_PTR(void, __fastcall, fn, 0x0140562690, hkReferencedObject * This);
			fn(this);
		}

		// TODO: See what the x64 equivalent of this would be, or write assembly / include a library.
#if 0
		__forceinline void* __cdecl operator new[](hk_size_t, void* p)
		{
			__asm { int 3 };
			return p;
		}
		__forceinline void __cdecl operator delete[](void*, void*)
		{
			__asm { int 3 };
		}
#endif
	};

	class hkpShapeContainer
	{
	public:
		virtual ~hkpShapeContainer() = default;
	};

	class hkpMeshMaterial
	{
	public:
		hkUint32 filterInfo;   //0x1419D22E8
	};

	//class hkPackfileEnum
	//{
	//public:
	//	hkInt8 storage;   //0x1419D1F48
	//};

	class hkAabb
	{
	public:
		hkVector4 min;   //0x1419D1888
		hkVector4 max;   //0x1419D1888
	};

	class hkpCollisionFilter : public hkReferencedObject
	{
	public:
		hkUint32 prepad[2];   //0x1419E3F90
		hkUint32 type;   //0x1419E4120
		hkUint32 postpad[3];   //0x1419D8EB0
	};



	template <class T>
	class hkRefPtr
	{
	public:
		T* m_pntr;

		/// Constructor from pointer.
		/// Increase reference count for object 'e' and set the pointer to it.
		HK_FORCE_INLINE hkRefPtr(T* e)
		{
			if (e)
			{
				e->addReference();
			}
			m_pntr = e;
		}

		HK_FORCE_INLINE hkRefPtr() : m_pntr(0) {}

		/// Destructor.
		/// Decrease reference count for stored object.
		HK_FORCE_INLINE ~hkRefPtr()
		{
			if (m_pntr)
			{
				m_pntr->removeReference();
			}
			m_pntr = nullptr;
		}
		/// Assignment operator.
		/// Increase reference count for object 'e',
		/// decrease reference count for stored object and set pointer to 'e'.
		HK_FORCE_INLINE void operator=(T* e)
		{
			if (e)
			{
				e->addReference(); // add reference first to allow self-assignment
			}
			if (m_pntr)
			{
				m_pntr->removeReference();
			}
			m_pntr = e;
		}

		/// Return pointer to stored object.
		HK_FORCE_INLINE T* val() const;

		/// Pointer to stored object.
		HK_FORCE_INLINE T* operator->() const;

		/// Replace stored pointer with 'e' without incrementing reference count for 'e'.
		/// Reference count for previously stored object is decreased.
		HK_FORCE_INLINE void setAndDontIncrementRefCount(T* e);

		/// Return pointer to stored object.
		HK_FORCE_INLINE operator T* () const;
	};

	//template <typename T>
	//class hkRefPtr
	//{
	//public:
	//	T* ptr;   //0x1419D9E10
	//};

	class hkpConvexListFilter : public hkReferencedObject
	{
	public:
	};

	class hkWorldMemoryAvailableWatchDog : public hkReferencedObject
	{
	public:
	};

	class hkpWorldCinfo : public hkReferencedObject
	{
	public:
		hkVector4 gravity;   //0x1419D1888
		hkInt32 broadPhaseQuerySize;   //0x1419D2278
		hkReal contactRestingVelocity;   //0x1419D1668
		hkInt8 broadPhaseType;   //0x1419E9B90
		hkInt8 broadPhaseBorderBehaviour;   //0x1419EBE30
		hkBool mtPostponeAndSortBroadPhaseBorderCallbacks;   //0x1419D0A40
		hkAabb broadPhaseWorldAabb;   //0x1419D0EF0
		hkReal collisionTolerance;   //0x1419D1668
		hkRefPtr<hkpCollisionFilter> collisionFilter;   //0x1419EBFD0
		hkRefPtr<hkpConvexListFilter> convexListFilter;   //0x1419E9380
		hkReal expectedMaxLinearVelocity;   //0x1419D1668
		int sizeOfToiEventQueue;   //0x1419D99E0
		hkReal expectedMinPsiDeltaTime;   //0x1419D1668
		hkRefPtr<hkWorldMemoryAvailableWatchDog> memoryWatchDog;   //0x1419E9400
		hkInt32 broadPhaseNumMarkers;   //0x1419D2278
		hkInt8 contactPointGeneration;   //0x1419EAC60
		hkBool allowToSkipConfirmedCallbacks;   //0x1419D0A40
		hkReal solverTau;   //0x1419D1668
		hkReal solverDamp;   //0x1419D1668
		hkInt32 solverIterations;   //0x1419D2278
		hkInt32 solverMicrosteps;   //0x1419D2278
		hkReal maxConstraintViolation;   //0x1419D1668
		hkBool forceCoherentConstraintOrderingInSolver;   //0x1419D0A40
		hkReal snapCollisionToConvexEdgeThreshold;   //0x1419D1668
		hkReal snapCollisionToConcaveEdgeThreshold;   //0x1419D1668
		hkBool enableToiWeldRejection;   //0x1419D0A40
		hkBool enableDeprecatedWelding;   //0x1419D0A40
		hkReal iterativeLinearCastEarlyOutDistance;   //0x1419D1668
		hkInt32 iterativeLinearCastMaxIterations;   //0x1419D2278
		hkUint8 deactivationNumInactiveFramesSelectFlag0;   //0x1419D20F8
		hkUint8 deactivationNumInactiveFramesSelectFlag1;   //0x1419D20F8
		hkUint8 deactivationIntegrateCounter;   //0x1419D20F8
		hkBool shouldActivateOnRigidBodyTransformChange;   //0x1419D0A40
		hkReal deactivationReferenceDistance;   //0x1419D1668
		hkReal toiCollisionResponseRotateNormal;   //0x1419D1668
		hkBool useCompoundSpuElf;   //0x1419D0A40
		int maxSectorsPerMidphaseCollideTask;   //0x1419D99E0
		int maxSectorsPerNarrowphaseCollideTask;   //0x1419D99E0
		hkBool processToisMultithreaded;   //0x1419D0A40
		int maxEntriesPerToiMidphaseCollideTask;   //0x1419D99E0
		int maxEntriesPerToiNarrowphaseCollideTask;   //0x1419D99E0
		int maxNumToiCollisionPairsSinglethreaded;   //0x1419D99E0
		hkReal numToisTillAllowedPenetrationSimplifiedToi;   //0x1419D1668
		hkReal numToisTillAllowedPenetrationToi;   //0x1419D1668
		hkReal numToisTillAllowedPenetrationToiHigher;   //0x1419D1668
		hkReal numToisTillAllowedPenetrationToiForced;   //0x1419D1668
		hkBool enableDeactivation;   //0x1419D0A40
		hkInt8 simulationType;   //0x1419E9800
		hkBool enableSimulationIslands;   //0x1419D0A40
		hkUint32 minDesiredIslandSize;   //0x1419D22E8
		hkBool processActionsInSingleThread;   //0x1419D0A40
		hkBool allowIntegrationOfIslandsWithoutConstraintsInASeparateJob;   //0x1419D0A40
		hkReal frameMarkerPsiSnap;   //0x1419D1668
		hkBool fireCollisionCallbacks;   //0x1419D0A40
	};

	class hkcdShape : public hkReferencedObject
	{
	public:
		hkInt8 type;   //0x1419E2CA0
		hkUint8 dispatchType;   //0x1419E2EB0
		hkUint8 bitsPerKey;   //0x1419D20F8
		hkUint8 shapeInfoCodecType;   //0x1419E2FB0
	};

	class hkpShapeBase : public hkcdShape
	{
	public:
	};

	class hkpShape : public hkpShapeBase
	{
	public:
		hkUlong userData;   //0x1419D2548
	};

	class hkpSphereRepShape : public hkpShape
	{
	};

	class hkpConvexShape : public hkpSphereRepShape
	{
	public:
		static constexpr float ms_DefaultRadius = 0.05f;
	public:
		float radius;
	};

	class hkpCdBody
	{
	public:
		hkpShape* shape;   //0x1419E4540
		unsigned int shapeKey;   //0x1419D9A40
		void* motion;   //0x1419CC3C0
		hkpCdBody* parent;   //0x1419E4760
	};

	class hkpBroadPhaseHandle
	{
	public:
		hkUint32 id;   //0x1419D22E8
	};

	class hkpTypedBroadPhaseHandle : public hkpBroadPhaseHandle
	{
	public:
		hkInt8 type;   //0x1419D1F48
		hkInt8 ownerOffset;   //0x1419D1F48
		hkInt8 objectQualityType;   //0x1419D1F48
		hkUint32 collisionFilterInfo;   //0x1419D22E8
	};

	class hkpCollidable : public hkpCdBody
	{
	public:
		class BoundingVolumeData
		{
		public:
			hkUint32 min[3];   //0x1419D8EB0
			hkUint8 expansionMin[3];   //0x1419D8FC0
			hkUint8 expansionShift;   //0x1419D20F8
			hkUint32 max[3];   //0x1419D8EB0
			hkUint8 expansionMax[3];   //0x1419D8FC0
			hkUint8 padding;   //0x1419D20F8
			hkUint16 numChildShapeAabbs;   //0x1419D21F8
			hkUint16 capacityChildShapeAabbs;   //0x1419D21F8
			void* childShapeAabbs;   //0x1419CC3C0
			void* childShapeKeys;   //0x1419CC3C0
		};

		hkInt8 ownerOffset;   //0x1419D1F48
		hkpTypedBroadPhaseHandle broadPhaseHandle;   //0x1419E12E0
		BoundingVolumeData boundingVolumeData;   //0x1419E1460
		hkReal allowedPenetrationDepth;   //0x1419D1668
	};

	class hkpLinkedCollidable : public hkpCollidable
	{
	public:
		hkArray<void*> collisionEntries;   //0x1419E43B0
	};

	class hkMultiThreadCheck
	{
	public:
		hkUint32 threadId;   //0x1419D22E8
		int stackTraceId;   //0x1419D99E0
		hkUint16 markCount;   //0x1419D21F8
		hkUint16 markBitStack;   //0x1419D21F8
	};

	class hkStringPtr
	{
	public:
		enum : char
		{
			OWNED_FLAG = 1
		};

		const char* stringAndFlag = nullptr;   //0x1419D9CE0

		static void assign(const char** ptr, const char* src)
		{
			if (((uint64_t)(*ptr) & ~OWNED_FLAG) == (uint64_t)src)
				return;

			if (((uint64_t)(*ptr) & OWNED_FLAG) == OWNED_FLAG)
				HkDealloc(0, const_cast<char*>(*ptr) - 1, 0);

			if (!src)
			{
				*ptr = "";
				return;
			}

			char* p = (char*)HkAlloc(0, strlen(src) + 1, 0);
			*ptr = p + OWNED_FLAG;
		}

		hkStringPtr& operator=(const char* string)
		{
			assign(&stringAndFlag, string);
			return *this;
		}

		hkStringPtr& operator=(const hkStringPtr& string)
		{
			assign(&stringAndFlag, string.stringAndFlag);
			return *this;
		}
	};

	class hkSimplePropertyValue
	{
	public:
		hkShared::hkUint64 data;   //0x1419D24C8
	};

	class hkSimpleProperty
	{
	public:
		hkUint32 key;   //0x1419D22E8
		hkUint32 alignmentPadding;   //0x1419D22E8
		hkSimplePropertyValue value;   //0x1419D1600
	};

	class hkpWorldObject : public hkReferencedObject
	{
	public:
		void* world;   //0x1419CC3C0
		hkpLinkedCollidable collidable;   //0x1419E10E0
		hkUlong userData;   //0x1419D2548
		hkMultiThreadCheck multiThreadCheck;   //0x1419D3F10
		hkStringPtr name;   //0x1419D5510
		hkArray<hkSimpleProperty> properties;   //0x1419E9FA0
	};
	ASSERT_SIZEOF(hkpWorldObject, 0x0D0);

	class hkpMaterial
	{
	public:
		hkInt8 responseType;   //0x1419E9680
		hkHalf16 rollingFrictionMultiplier;   //0x1419D0690
		hkReal friction;   //0x1419D1668
		hkReal restitution;   //0x1419D1668
	};

	class hkpConstraintData : public hkReferencedObject
	{
	public:
		hkUlong userData;   //0x1419D2548
		hkUint8 numNonDirectlySolvedAtoms;   //0x1419D20F8
	};

	class hkpConstraintAtom
	{
	public:
		hkUint16 type;   //0x1419EAEF0
	};

	class hkpModifierConstraintAtom : public hkpConstraintAtom
	{
	public:
		hkUint16 modifierAtomSize;   //0x1419D21F8
		hkUint16 childSize;   //0x1419D21F8
		hkpConstraintAtom* child;   //0x1419EA030
		hkUint32 pad[2];   //0x1419E3F90
	};

	class hkpConstraintInstance : public hkReferencedObject
	{
	public:
		class SmallArraySerializeOverrideType
		{
		public:
			void* data;   //0x1419D9E10
			hkUint16 size;   //0x1419D21F8
			hkUint16 capacityAndFlags;   //0x1419D21F8
		};

		void* owner;   //0x1419CC3C0
		hkpConstraintData* data;   //0x1419EAF70
		hkpModifierConstraintAtom* constraintModifiers;   //0x1419EB070
		hkpEntity* entities[2];   //0x1419EB270
		hkUint8 priority;   //0x1419EB520
		hkBool wantRuntime;   //0x1419D0A40
		hkUint8 destructionRemapInfo;   //0x1419EB750
		SmallArraySerializeOverrideType listeners;   //0x1419E8A10
		hkStringPtr name;   //0x1419D5510
		hkUlong userData;   //0x1419D2548
		void* internal;   //0x1419CC3C0
		hkUint32 uid;   //0x1419D22E8
	};

	class hkViewPtr
	{
	public:
		void* ptr;   //0x1419D9E10
	};

	class hkUFloat8
	{
	public:
		hkUint8 value;   //0x1419D20F8
	};

	class hkMotionState
	{
	public:
		hkTransform transform;   //0x1419D1A78
		hkVector4f sweptTransform[5];   //0x1419EA230
		hkVector4 deltaAngle;   //0x1419D1888
		hkReal objectRadius;   //0x1419D1668
		hkHalf16 linearDamping;   //0x1419D0690
		hkHalf16 angularDamping;   //0x1419D0690
		hkHalf16 timeFactor;   //0x1419D0690
		hkUFloat8 maxLinearVelocity;   //0x1419D3AB0
		hkUFloat8 maxAngularVelocity;   //0x1419D3AB0
		hkUint8 deactivationClass;   //0x1419D20F8
	};

	class hkpMotion : public hkReferencedObject
	{
	public:
		enum MotionType : uint8_t
		{
			MOTION_INVALID = 0x0,
			MOTION_DYNAMIC = 0x1,
			MOTION_SPHERE_INERTIA = 0x2,
			MOTION_BOX_INERTIA = 0x3,
			MOTION_KEYFRAMED = 0x4,
			MOTION_FIXED = 0x5,
			MOTION_THIN_BOX_INERTIA = 0x6,
			MOTION_CHARACTER = 0x7,
			MOTION_MAX_ID = 0x8,
		};

		MotionType type;   //0x1419EB390
		hkUint8 deactivationIntegrateCounter;   //0x1419D20F8
		hkUint16 deactivationNumInactiveFrames[2];   //0x1419EB490
		hkMotionState motionState;   //0x1419E4B30
		hkVector4 inertiaAndMassInv;   //0x1419D1888
		hkVector4 linearVelocity;   //0x1419D1888
		hkVector4 angularVelocity;   //0x1419D1888
		hkVector4 deactivationRefPosition[2];   //0x1419EB6C0
		hkUint32 deactivationRefOrientation[2];   //0x1419E3F90
		hkpMotion* savedMotion;   //0x1419EB8E0
		hkUint16 savedQualityTypeIndex;   //0x1419D21F8
		hkHalf16 gravityFactor;   //0x1419D0690
	};

	class hkpKeyframedRigidMotion : public hkpMotion
	{
	public:
	};

	class hkpMaxSizeMotion : public hkpKeyframedRigidMotion
	{
	public:
	};

	class hkLocalFrame : public hkReferencedObject
	{
	public:
	};


	class hkpEntity : public hkpWorldObject
	{
	public:
		class SpuCollisionCallback
		{
		public:
			void* util;   //0x1419CC3C0
			hkUint16 capacity;   //0x1419D21F8
			hkUint8 eventFilter;   //0x1419D20F8
			hkUint8 userFilter;   //0x1419D20F8
		};

		class __declspec(align(16)) SmallArraySerializeOverrideType
		{
		public:
			void* data;   //0x1419D9E10
			hkUint16 size;   //0x1419D21F8
			hkUint16 capacityAndFlags;   //0x1419D21F8
		};

		class ExtendedListeners
		{
		public:
			SmallArraySerializeOverrideType activationListeners;   //0x1419E5290
			SmallArraySerializeOverrideType entityListeners;   //0x1419E5290
		};

		hkpMaterial material;   //0x1419E6800
		void* limitContactImpulseUtilAndFlag;   //0x1419D9E10
		hkReal damageMultiplier;   //0x1419D1668
		void* breakableBody;   //0x1419CC3C0
		hkUint32 solverData;   //0x1419D22E8
		unsigned short storageIndex;   //0x1419D9980
		hkUint16 contactPointCallbackDelay;   //0x1419D21F8
		SmallArraySerializeOverrideType constraintsMaster;   //0x1419E5290
		hkArray<hkViewPtr> constraintsSlave;   //0x1419E91E0
		hkArray<hkUint8> constraintRuntime;   //0x1419CDF10
		void* simulationIsland;   //0x1419CC3C0
		hkInt8 autoRemoveLevel;   //0x1419D1F48
		hkUint8 numShapeKeysInContactPointProperties;   //0x1419D20F8
		hkUint8 responseModifierFlags;   //0x1419D20F8
		hkUint32 uid;   //0x1419D22E8
		SpuCollisionCallback spuCollisionCallback;   //0x1419E5E40
		hkpMaxSizeMotion motion;   //0x1419E5040
		SmallArraySerializeOverrideType contactListeners;   //0x1419E5290
		SmallArraySerializeOverrideType actions;   //0x1419E5290
		hkRefPtr<hkLocalFrame> localFrame {};   //0x1419DF0A0
		ExtendedListeners* extendedListeners;   //0x1419E9A90
	};


	ASSERT_SIZEOF(hkpEntity::SmallArraySerializeOverrideType, 0x10);
	ASSERT_SIZEOF(hkpEntity::SmallArraySerializeOverrideType, 0x10);
	ASSERT_SIZEOF(hkpEntity, 0x2C0);
	ASSERT_SIZEOF(hkpMaterial, 0x0C);

	ASSERT_OFFSETOF(hkpEntity, material, 0xD0);
	ASSERT_OFFSETOF(hkpEntity, limitContactImpulseUtilAndFlag, 0xE0);
	ASSERT_OFFSETOF(hkpEntity, damageMultiplier, 0xE8);
	ASSERT_OFFSETOF(hkpEntity, breakableBody, 0xF0);
	ASSERT_OFFSETOF(hkpEntity, solverData, 0xF8);
	ASSERT_OFFSETOF(hkpEntity, storageIndex, 0xFC);
	ASSERT_OFFSETOF(hkpEntity, contactPointCallbackDelay, 0xFE);
	ASSERT_OFFSETOF(hkpEntity, constraintsMaster, 0x100);
	ASSERT_OFFSETOF(hkpEntity, constraintsSlave, 0x110);
	ASSERT_OFFSETOF(hkpEntity, constraintRuntime, 0x120);
	ASSERT_OFFSETOF(hkpEntity, simulationIsland, 0x130);
	ASSERT_OFFSETOF(hkpEntity, autoRemoveLevel, 0x138);

	ASSERT_OFFSETOF(hkpEntity, localFrame, 0x2B0);
	ASSERT_OFFSETOF(hkpEntity, extendedListeners, 0x2B8);

	struct hkpRigidBodyCinfo
	{
		int m_collisionFilterInfo = 0;
		hkpShape* m_shape;
		hkVector4f m_position {};
		hkQuaternionf m_rotation {};
		hkVector4f m_linearVelocity {};
		hkVector4f m_angularVelocity {};
		hkVector4f m_inertiaTensor[3];
		hkVector4f m_centerOfMass {};
		float m_mass = 0;
		float m_linearDampening = 0;
		float m_angularDamping = 0.05f;
		float m_gravityFactor = 1.0f;
		float m_friction = 0.5f;
		float m_rollingFrictionMultiplier = 0;
		float m_restitution = 0.4f;
		float m_maxLinearVelocity = 200.0f;
		float m_maxAngularVelocity = 200.0f;
		hkpMotion::MotionType m_motionType = hkpMotion::MOTION_DYNAMIC;
		char m_enableDeactivation = 1;
		char m_solverDeactivation = 2;
		float allowedPenetrationDepth = -1.0f;
		float m_timeFactor = 1.0f;
		void* m_localFrame = nullptr;
		char m_collisionResponse = 1;
		int16_t m_contactPointCallbackDelay = -1;
		char m_qualityType = -1;
		char m_autoRemoveLevel = 0;
		char m_responseModifierFlags = 0;
		char m_numShapeKeysInContactPointProperties = 0;

		hkpRigidBodyCinfo(hkpShape* shape) : m_shape(shape)
		{
			m_inertiaTensor[0] = { 1,0,0,0 };
			m_inertiaTensor[1] = { 0,1,0,0 };
			m_inertiaTensor[2] = { 0,0,1,0 };
		}
		hkpRigidBodyCinfo() : hkpRigidBodyCinfo(nullptr) {}
	};

	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_collisionFilterInfo, 0x00);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_shape, 0x08);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_position, 0x10);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_rotation, 0x20);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_linearVelocity, 0x30);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_angularVelocity, 0x40);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_inertiaTensor, 0x50);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_centerOfMass, 0x80);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_mass, 0x90);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_linearDampening, 0x94);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_angularDamping, 0x98);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_gravityFactor, 0x9C);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_friction, 0xA0);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_rollingFrictionMultiplier, 0xA4);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_restitution, 0xA8);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_maxLinearVelocity, 0xAC);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_maxAngularVelocity, 0xB0);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_motionType, 0xB4);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_enableDeactivation, 0xB5);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_solverDeactivation, 0xB6);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, allowedPenetrationDepth, 0xB8);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_timeFactor, 0xBC);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_localFrame, 0xC0);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_collisionResponse, 0xC8);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_contactPointCallbackDelay, 0xCA);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_qualityType, 0xCC);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_autoRemoveLevel, 0xCD);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_responseModifierFlags, 0xCE);
	ASSERT_OFFSETOF(hkpRigidBodyCinfo, m_numShapeKeysInContactPointProperties, 0xCF);

	class hkpRigidBody : public hkpEntity
	{
	public:
		hkpRigidBody(const hkpRigidBodyCinfo& info)
		{
			FUNCTION_PTR(void, __fastcall, ctor, pHkpRigidBodyCtor, void* This, const hkpRigidBodyCinfo & info);
			ctor(this, info);
		}
	};

	class hkpAction : public hkReferencedObject
	{
	public:
		void* world;   //0x1419CC3C0
		void* island;   //0x1419CC3C0
		hkUlong userData;   //0x1419D2548
		hkStringPtr name;   //0x1419D5510
	};

	class hkpPhantom : public hkpWorldObject
	{
	public:
		hkArray<void*> overlapListeners;   //0x1419E43B0
		hkArray<void*> phantomListeners;   //0x1419E43B0
	};

	class hkpPhysicsSystem : public hkReferencedObject
	{
	public:
		hkArray<hkpRigidBody*> rigidBodies {};   //0x1419CF380
		hkArray<hkpConstraintInstance*> constraints {};   //0x1419CF490
		hkArray<hkpAction*> actions {};   //0x1419EB630
		hkArray<hkpPhantom*> phantoms {};   //0x1419EBA70
		hkStringPtr name {};   //0x1419D5510
		hkUlong userData {};   //0x1419D2548
		hkBool active {};   //0x1419D0A40

		hkpPhysicsSystem()
		{
			FUNCTION_PTR(void, __fastcall, ctor, pHkPhysicsSystemCtor, void* This);
			ctor(this);
		}
	};

	class hkpPhysicsData : public hkReferencedObject
	{
	public:
		hkpWorldCinfo* worldCinfo;   //0x1419ECAD0
		hkArray<hkpPhysicsSystem*> systems;   //0x1419EE1C0

		hkpPhysicsData()
		{
			FUNCTION_PTR(void, __fastcall, ctor, pHkPhysicsDataCtor, void* This);
			ctor(this);
		}
	};

	class hkpShapeCollection /* 0x1419E2250 */ : public hkpShape /* 1419E06B0 */, public hkpShapeContainer /* 1419E20B0 */
	{
	public:
		hkBool disableWelding;   //0x1419D0A40
		hkUint8 collectionType;   //0x1419E3AB0
	};

	class hkpMeshShape : public hkpShapeCollection
	{
	public:
		class Subpart
		{
		public:
			// Note: opaque types were derived from hk2010_2_0 header information.

			float* vertexBase;   //0x1419CC3C0
			int vertexStriding;   //0x1419D99E0
			int numVertices;   //0x1419D99E0
			void* indexBase;   //0x1419CC3C0  -- "A pointer to triples of vertex indices."
			hkInt8 stridingType;   //0x1419E42A0
			hkInt8 materialIndexStridingType;   //0x1419E4440
			int indexStriding;   //0x1419D99E0
			int flipAlternateTriangles;   //0x1419D99E0
			int numTriangles;   //0x1419D99E0
			/// Pointer to a strided array of material index (hkUint8 or hkUint16), one index for each triangle.
			///  - You are limited to a maximum of 256 or 65535 materials per subpart.
			///  - The indices may be stored in an interleaved array by setting m_materialIndexStriding appropriately.
			///  - If you do not want to use materials, simply set this element to HK_NULL
			void* materialIndexBase;   //0x1419CC3C0
			int materialIndexStriding;   //0x1419D99E0

			hkpMeshMaterial* materialBase;   //0x1419CC3C0
			int materialStriding;   //0x1419D99E0
			int numMaterials;   //0x1419D99E0
			int triangleOffset;   //0x1419D99E0
		};

		hkVector4 scaling;   //0x1419D1888
		hkInt32 numBitsForSubpartIndex;   //0x1419D2278
		hkArray<Subpart> subparts;   //0x1419E3F00
		hkArray<hkUint16> weldingInfo;   //0x1419CE1C0
		hkUint8 weldingType;   //0x1419E3030
		hkReal radius;   //0x1419D1668
		int pad[3];   //0x1419CE970
	};

	class hkpFastMeshShape : public hkpMeshShape
	{
	public:
	};

	class hkpExtendedMeshShape /* 0x1419E0ED0 */ : public hkpShapeCollection /* 1419E2250 */
	{
	public:
		enum IndexStridingType : hkInt8
		{
			INDICES_INVALID,	///< default, will raise assert.
			INDICES_INT8,		///< 8 bit "single byte" striding.
			INDICES_INT16,		///< 16 bit "short" striding.
			INDICES_INT32,		///< 32 bit "int" striding.
			INDICES_MAX_ID
		};

		enum MaterialIndexStridingType : hkInt8
		{
			MATERIAL_INDICES_INVALID = 0,
			MATERIAL_INDICES_INT8 = 1,
			MATERIAL_INDICES_INT16 = 2,
			MATERIAL_INDICES_MAX_ID = 3,
		};

		enum SubpartType : hkInt8
		{
			SUBPART_TRIANGLES = 0,
			SUBPART_SHAPE = 1,
			SUBPART_TYPE_MAX = 2,
		};

		enum SubpartTypesAndFlags : hkInt8
		{
			SUBPART_TYPE_MASK = 0x0001,	// Subpart type stored in the first bit
			SUBPART_MATERIAL_INDICES_MASK = 0x0006,	// Bits 2 and 3 used for material indices
			SUBPART_MATERIAL_INDICES_SHIFT = 1,
			SUBPART_NUM_MATERIALS_MASK = 0xFFF8,	// Most significant 13 bits used for storing the number of materials
			SUBPART_NUM_MATERIALS_SHIFT = 3,
		};

		class Subpart
		{
		public:
			enum Flags : hkUint16
			{
				SUBPART_TYPE_MASK = 0x0001,             // Subpart type stored in the first bit
				SUBPART_MATERIAL_INDICES_MASK = 0x0006, // Bits 2 and 3 used for material indices
				SUBPART_MATERIAL_INDICES_SHIFT = 1,
				SUBPART_NUM_MATERIALS_MASK = 0xFFF8,    // Most significant 13 bits used for storing the number of materials
				SUBPART_NUM_MATERIALS_SHIFT = 3,
			};

			hkUint16 typeAndFlags;   //0x1419D21F8
			hkUint16 shapeInfo;   //0x1419D21F8
			hkInt16 materialStriding;   //0x1419D2178
			hkUint16 materialIndexStriding;   //0x1419D21F8
			void* materialIndexBase;   //0x1419CC3C0
			hkpMeshMaterial* materialBase;   //0x1419CC3C0
			hkUlong userData;   //0x1419D2548

			HK_FORCE_INLINE void setType(hkInt8 newValue)
			{
				typeAndFlags = (typeAndFlags & ~SUBPART_TYPE_MASK) | ((hkUint16)newValue & SUBPART_TYPE_MASK);
			}
			HK_FORCE_INLINE void setMaterialIndexStridingType(hkInt8 newValue)
			{
				typeAndFlags = (hkUint16)((typeAndFlags & (~SUBPART_MATERIAL_INDICES_MASK)) | ((newValue << SUBPART_MATERIAL_INDICES_SHIFT) & SUBPART_MATERIAL_INDICES_MASK));
			}
			HK_FORCE_INLINE void setNumMaterials(hkUint16 newValue)
			{
				typeAndFlags = (hkUint16)((typeAndFlags & (~SUBPART_NUM_MATERIALS_MASK)) | ((newValue << SUBPART_NUM_MATERIALS_SHIFT) & SUBPART_NUM_MATERIALS_MASK));
			}

			// Implement these MAYBE.
			//HK_FORCE_INLINE SubpartType getType() const;
			//HK_FORCE_INLINE MaterialIndexStridingType getMaterialIndexStridingType() const;
			//HK_FORCE_INLINE hkUint16 getNumMaterials() const;
		};

		class ShapesSubpart : public Subpart
		{
		public:
			hkArray<hkRefPtr<hkpConvexShape>> childShapes;   //0x1419E3810
			hkQuaternion rotation;   //0x1419D1A18
			hkVector4 translation;   //0x1419D1888
		};

		class TrianglesSubpart : public Subpart
		{
		public:
			int numTriangleShapes;   //0x1419D99E0
			void* vertexBase;   //0x1419CC3C0 - NoSave
			int numVertices;   //0x1419D99E0
			void* indexBase;   //0x1419CC3C0 - NoSave
			hkUint16 vertexStriding;   //0x1419D21F8
			int triangleOffset;   //0x1419D99E0
			hkUint16 indexStriding;   //0x1419D21F8
			hkInt8 stridingType;   //0x1419E33F0
			hkInt8 flipAlternateTriangles;   //0x1419D1F48
			hkVector4 extrusion;   //0x1419D1888
			hkQsTransform transform;   //0x1419D1BC8
		};

		TrianglesSubpart embeddedTrianglesSubpart;   //0x1419E1080
		hkVector4 aabbHalfExtents;   //0x1419D1888
		hkVector4 aabbCenter;   //0x1419D1888
		void* materialClass;   //0x1419CC3C0
		hkInt32 numBitsForSubpartIndex;   //0x1419D2278
		hkArray<TrianglesSubpart> trianglesSubparts;   //0x1419E27D0
		hkArray<ShapesSubpart> shapesSubparts;   //0x1419E2A80
		hkArray<hkUint16> weldingInfo;   //0x1419CE1C0
		hkUint8 weldingType;   //0x1419E3030
		hkUint32 defaultCollisionFilterInfo;   //0x1419D22E8
		hkInt32 cachedNumChildShapes;   //0x1419D2278
		hkReal triangleRadius;   //0x1419D1668
		hkInt32 padding;   //0x1419D2278
	};

	class hkpListShape /* 0x1419E1F80 */ : public hkpShapeCollection
	{
	public:
		class ChildInfo
		{
		public:
			hkpShape* shape;   //0x1419E4540
			hkUint32 collisionFilterInfo;   //0x1419D22E8
			hkUint16 shapeInfo;   //0x1419D21F8
			hkInt16 shapeSize;   //0x1419D2178
			int numChildShapes;   //0x1419D99E0
		};

		hkArray<ChildInfo> childInfo;   //0x1419E3DE0
		hkUint16 flags;   //0x1419D21F8
		hkUint16 numDisabledChildren;   //0x1419D21F8
		hkVector4 aabbHalfExtents;   //0x1419D1888
		hkVector4 aabbCenter;   //0x1419D1888
		hkUint32 enabledChildren[8];   //0x1419E3E70
	};

	

	class hkpSimpleMeshShape : public hkpShapeCollection
	{
	public:
		class Triangle
		{
		public:
			int a;   //0x1419D99E0
			int b;   //0x1419D99E0
			int c;   //0x1419D99E0
			hkUint16 weldingInfo;   //0x1419D21F8
		};

		hkArray<hkVector4> vertices;   //0x1419CEBA0
		hkArray<Triangle> triangles;   //0x1419E46D0
		hkArray<hkUint8> materialIndices;   //0x1419CDF10
		hkReal radius;   //0x1419D1668
		hkUint8 weldingType;   //0x1419E3030
	};


	class hkpNamedMeshMaterial : public hkpMeshMaterial
	{
	public:
		hkStringPtr name;   //0x1419D5510
	};


	class hkpStorageExtendedMeshShape /* 1419E04D0 */ : public hkpExtendedMeshShape
	{
	public:

		class Material : public hkpMeshMaterial
		{
		public:
			hkHalf16 restitution;   //0x1419D0690
			hkHalf16 friction;   //0x1419D0690
			hkUlong userData;   //0x1419D2548
		};

		class MeshSubpartStorage : public hkReferencedObject
		{
		public:
			hkArray<hkVector4> vertices;   //0x1419CEBA0
			hkArray<hkUint8> indices8;   //0x1419CDF10
			hkArray<hkUint16> indices16;   //0x1419CE1C0
			hkArray<hkUint32> indices32;   //0x1419CEFC0
			hkArray<hkUint8> materialIndices;   //0x1419CDF10
			hkArray<Material> materials;   //0x1419E3360
			hkArray<hkpNamedMeshMaterial> namedMaterials;   //0x1419E3580
			hkArray<hkUint16> materialIndices16;   //0x1419CE1C0
		};

		class ShapeSubpartStorage : public hkReferencedObject
		{
		public:
			hkArray<hkUint8> materialIndices;   //0x1419CDF10
			hkArray<Material> materials;   //0x1419E3360
			hkArray<hkUint16> materialIndices16;   //0x1419CE1C0
		};

		hkArray<MeshSubpartStorage*> meshstorage;   //0x1419E2D20
		hkArray<ShapeSubpartStorage*> shapestorage;   //0x1419E30B0

		hkpStorageExtendedMeshShape(float radius, int in_numBitsForSubpartIndex = 12)
		{
			FUNCTION_PTR(void, __fastcall, ctor, pHkpStorageExtendedMeshShapeCtor, void* This, float _radius, int _numBits);
			ctor(this, radius, in_numBitsForSubpartIndex);
		}

		hkpStorageExtendedMeshShape(int in_numBitsForSubpartIndex = 12)
		{
			FUNCTION_PTR(void, __fastcall, ctor, pHkpStorageExtendedMeshShapeCtor, void* This, float _radius, int _numBits);
			ctor(this, hkpConvexShape::ms_DefaultRadius, in_numBitsForSubpartIndex);
		}

		MAKE_DEEP_COPY_FUNC(hkpStorageExtendedMeshShape);
	};

	class hkpSingleShapeContainer : public hkpShapeContainer
	{
	public:
		hkpShape* childShape;   //0x1419E4540
	};

	class hkpCompressedMeshShape : public hkpShapeCollection
	{
	public:
		class BigTriangle
		{
		public:
			hkUint16 a;   //0x1419D21F8
			hkUint16 b;   //0x1419D21F8
			hkUint16 c;   //0x1419D21F8
			hkUint32 material;   //0x1419D22E8
			hkUint16 weldingInfo;   //0x1419D21F8
			hkUint16 transformIndex;   //0x1419D21F8
		};

		class ConvexPiece
		{
		public:
			hkVector4 offset;   //0x1419D1888
			hkArray<hkUint16> vertices;   //0x1419CE1C0
			hkUint16 reference;   //0x1419D21F8
			hkUint16 transformIndex;   //0x1419D21F8
		};

		class Chunk
		{
		public:
			hkVector4 offset;   //0x1419D1888
			hkArray<hkUint16> vertices;   //0x1419CE1C0
			hkArray<hkUint16> indices;   //0x1419CE1C0
			hkArray<hkUint16> stripLengths;   //0x1419CE1C0
			hkArray<hkUint16> weldingInfo;   //0x1419CE1C0
			hkUint32 materialInfo;   //0x1419D22E8
			hkUint16 reference;   //0x1419D21F8
			hkUint16 transformIndex;   //0x1419D21F8
		};

		int bitsPerIndex;   //0x1419D99E0
		int bitsPerWIndex;   //0x1419D99E0
		int wIndexMask;   //0x1419D99E0
		int indexMask;   //0x1419D99E0
		hkReal radius;   //0x1419D1668
		hkUint8 weldingType;   //0x1419E3030
		hkUint8 materialType;   //0x1419E47E0
		hkArray<hkUint32> materials;   //0x1419CEFC0
		hkArray<hkUint16> materials16;   //0x1419CE1C0
		hkArray<hkUint8> materials8;   //0x1419CDF10
		hkArray<hkQsTransform> transforms;   //0x1419CD770
		hkArray<hkVector4> bigVertices;   //0x1419CEBA0
		hkArray<BigTriangle> bigTriangles;   //0x1419E2740
		hkArray<Chunk> chunks;   //0x1419E29F0
		hkArray<ConvexPiece> convexPieces;   //0x1419E2B90
		hkReal error;   //0x1419D1668
		hkAabb bounds;   //0x1419D0EF0
		hkUint32 defaultCollisionFilterInfo;   //0x1419D22E8
		void* meshMaterials;   //0x1419CC3C0
		hkUint16 materialStriding;   //0x1419D21F8
		hkUint16 numMaterials;   //0x1419D21F8
		hkArray<hkpNamedMeshMaterial> namedMaterials;   //0x1419E3580
	};

	class hkpConvexListShape /* 0x1419E2430 */ : public hkpConvexShape /* 1419E0C20 */, public hkpShapeContainer /* 1419E20B0 */
	{
	public:
		hkReal minDistanceToUseConvexHullForGetClosestPoints;   //0x1419D1668
		hkVector4 aabbHalfExtents;   //0x1419D1888
		hkVector4 aabbCenter;   //0x1419D1888
		hkBool useCachedAabb;   //0x1419D0A40
		hkArray<hkpConvexShape*> childShapes;   //0x1419E3250
	};

	class hkpConvexPieceStreamData : public hkReferencedObject
	{
	public:
		hkArray<hkUint32> convexPieceStream;   //0x1419CEFC0
		hkArray<hkUint32> convexPieceOffsets;   //0x1419CEFC0
		hkArray<hkUint32> convexPieceSingleTriangles;   //0x1419CEFC0
	};

	class hkpConvexPieceMeshShape : public hkpShapeCollection
	{
	public:
		hkpConvexPieceStreamData* convexPieceStream;   //0x1419E3690
		hkpShapeCollection* displayMesh;   //0x1419E38A0
		hkReal radius;   //0x1419D1668
	};

	class hkpStorageMeshShape : public hkpMeshShape
	{
	public:
		class SubpartStorage : public hkReferencedObject
		{
		public:
			hkArray<hkReal> vertices;   //0x1419CD890
			hkArray<hkUint16> indices16;   //0x1419CE1C0
			hkArray<hkUint32> indices32;   //0x1419CEFC0
			hkArray<hkUint8> materialIndices;   //0x1419CDF10
			hkArray<hkUint32> materials;   //0x1419CEFC0
			hkArray<hkUint16> materialIndices16;   //0x1419CE1C0
		};

		hkArray<SubpartStorage*> storage;   //0x1419E2620
	};

	class hkpHeightFieldShape : public hkpShape
	{
	public:
	};


	class hkpSampledHeightFieldShape : public hkpHeightFieldShape
	{
	public:
		class CoarseMinMaxLevel
		{
		public:
			hkArray<hkVector4> minMaxData;   //0x1419CEBA0
			int xRes;   //0x1419D99E0
			int zRes;   //0x1419D99E0
		};

		hkArray<CoarseMinMaxLevel> coarseTreeData;   //0x1419E3140
		int coarseness;   //0x1419D99E0
		hkReal raycastMinY;   //0x1419D1668
		hkReal raycastMaxY;   //0x1419D1668
		int xRes;   //0x1419D99E0
		int zRes;   //0x1419D99E0
		hkReal heightCenter;   //0x1419D1668
		hkBool useProjectionBasedHeight;   //0x1419D0A40
		hkUint8 heightfieldType;   //0x1419E32E0
		hkVector4 intToFloatScale;   //0x1419D1888
		hkVector4 floatToIntScale;   //0x1419D1888
		hkVector4 floatToIntOffsetFloorCorrected;   //0x1419D1888
		hkVector4 extents;   //0x1419D1888
	};

	class hkpTriSampledHeightFieldCollection : public hkpShapeCollection
	{
	public:
		hkpSampledHeightFieldShape* heightfield;   //0x1419E3D60
		int childSize;   //0x1419D99E0
		hkReal radius;   //0x1419D1668
		hkArray<hkUint16> weldingInfo;   //0x1419CE1C0
		hkVector4 triangleExtrusion;   //0x1419D1888
	};

	class hkpBvTreeShape : public hkpShape
	{
	public:
		hkUint8 bvTreeType;   //0x1419E28E0
	};

	template <typename T>
	class hkRelArray
	{
	public:
		~hkRelArray() {}

		hkInt64 offset;   //0x1419D2448
		int size;   //0x1419D99E0
		int capacityAndFlags;   //0x1419D99E0

		HK_FORCE_INLINE const T& operator [](int i) const
		{
			return (const T*)((uint64_t)this + offset)[i];
		}

		HK_FORCE_INLINE T& operator [](int i)
		{
			return (T*)((uint64_t)this + offset)[i];
		}

		HK_FORCE_INLINE hkRelArray(hkUint16 size, const void* ptr) : size(size)
		{
			offset = (uint64_t)ptr - (uint64_t)this;
		}
	};



	class hkcdStaticTree // Unknown type, only here for struct naming
	{
	public:
		class Aabb5BytesTree
		{
		public:
		};

		class Aabb6BytesTree
		{
		public:
		};
	};

	class hkcdStaticMeshTree : public hkcdStaticTree // Unknown type, only here for struct naming
	{
	public:
		class Section
		{
		public:
		};
		class Primitive
		{
		public:
			hkUint8 indices[4];   //0x1419EE830
		};

		class Base : public Aabb5BytesTree
		{
		public:
			int numPrimitiveKeys;   //0x1419D99E0
			int bitsPerKey;   //0x1419D99E0
			hkUint32 maxKeyValue;   //0x1419D22E8
			hkUint8 primitiveStoresIsFlatConvex;   //0x1419D20F8
			// I hope we never need to use these, cuz I have no clue how this type works!
			hkRelArray<Section> sections;   //0x141A04830
			hkRelArray<Primitive> primitives;   //0x141A049D0
			hkRelArray<hkUint16> sharedVerticesIndex;   //0x141A04C10
		};
	};

	class hkpBvCompressedMeshShapeTree : public hkcdStaticMeshTree::Base
	{
	public:
		class PrimitiveDataRun
		{
		public:
			hkUint32 value;   //0x1419D22E8
			hkUint8 index;   //0x1419D20F8
			hkUint8 count;   //0x1419D20F8
		};

		hkArray<unsigned int> packedVertices;   //0x1419CEA00
		hkArray<unsigned long long> sharedVertices;   //0x1419D7A50
		hkArray<PrimitiveDataRun> primitiveDataRuns;   //0x1419EC2D0
	};

	class hkpBvCompressedMeshShape /* 0x1419EC4A0 */ : public hkpBvTreeShape /* 1419E2500 */, public hkpShapeContainer /* 1419E20B0 */
	{
	public:
		hkReal convexRadius;   //0x1419D1668
		hkUint8 weldingType;   //0x1419E3030
		hkBool hasPerPrimitiveCollisionFilterInfo;   //0x1419D0A40
		hkBool hasPerPrimitiveUserData;   //0x1419D0A40
		hkArray<hkUint32> collisionFilterInfoPalette;   //0x1419CEFC0
		hkArray<hkUint32> userDataPalette;   //0x1419CEFC0
		hkArray<hkStringPtr> userStringPalette;   //0x1419DEEF0
		hkpBvCompressedMeshShapeTree tree;   //0x1419EC430
	};

	class hkpShapeKeyTable
	{
	public:
		class Block
		{
		public:
			hkUint32 slots[63];   //0x1419EC7C0
			Block* next;   //0x1419EC740
		};

		Block* lists;   //0x1419EC740
		hkUint32 occupancyBitField;   //0x1419D22E8
	};

	class hkpStaticCompoundShape : public hkpBvTreeShape
	{
	public:
		class Instance
		{
		public:
			hkQsTransform transform;   //0x1419D1BC8
			hkpShape* shape;   //0x1419E4540
			hkUint32 filterInfo;   //0x1419D22E8
			hkUint32 childFilterInfoMask;   //0x1419D22E8
			hkUlong userData;   //0x1419D2548
		};

		hkInt8 numBitsForChildShapeKey;   //0x1419D1F48
		hkInt8 referencePolicy;   //0x1419D1F48
		hkUint32 childShapeKeyMask;   //0x1419D22E8
		hkArray<Instance> instances;   //0x1419EC850
		hkArray<hkUint16> instanceExtraInfos;   //0x1419CE1C0
		hkpShapeKeyTable disabledLargeShapeKeyTable;   //0x1419EC580
		hkcdStaticTree::Aabb6BytesTree tree;   //0x141A02C78
	};

	class hkpBoxShape /* 0x1419E1020 */ : public hkpConvexShape /* 1419E0C20 */
	{
	public:
		hkVector4f halfExtents;

		hkpBoxShape(const hkVector4f& extents, float inRadius)
		{
			FUNCTION_PTR(void, __fastcall, ctor, pHkpBoxShapeCtor, void* This, const hkVector4f & extents, float radius);
			ctor(this, extents, inRadius);
		}

		hkpBoxShape(const hkVector4f& extents)
		{
			FUNCTION_PTR(void, __fastcall, ctor, pHkpBoxShapeCtor, void* This, const hkVector4f & extents, float radius);
			ctor(this, extents, ms_DefaultRadius);
		}

		MAKE_DEEP_COPY_FUNC(hkpBoxShape);

		//hkpBoxShape(const hkVector4f& extents) : hkpBoxShape(extents, ms_DefaultRadius) {}
	};
	ASSERT_SIZEOF(hkpBoxShape, 0x40);

	class hkpCapsuleShape /* 0x1419E1220 */ : public hkpConvexShape /* 1419E0C20 */
	{
	public:
		hkVector4 vertexA;   //0x1419D1888
		hkVector4 vertexB;   //0x1419D1888
	};

	class hkpConvexTransformShapeBase /* 0x1419E0C80 */ : public hkpConvexShape /* 1419E0C20 */
	{
	public:
		hkpSingleShapeContainer childShape;   //0x1419E2120
		int childShapeSizeForSpu;   //0x1419D99E0

		void DeepCopyChildClass(const hk2010_2_0::hkpConvexTransformShapeBase* in_pShape2010);
	};

	class hkpConvexTransformShape /* 0x1419E1140 */ : public hkpConvexTransformShapeBase /* 1419E0C80 */
	{
	public:
		hkQsTransform transform;   //0x1419D1BC8
		hkVector4 extraScale;   //0x1419D1888

		hkpConvexTransformShape()
		{
			FUNCTION_PTR(void, __fastcall, ctor, pHkpConvexTransformShapeCreate, hkpConvexTransformShape * This, int, int count);
			ctor(this, 0, 1);
		}

		MAKE_DEEP_COPY_FUNC(hkpConvexTransformShape);
	};

	class hkpConvexTranslateShape /* 0x1419E1330 */ : public hkpConvexTransformShapeBase /* 1419E0C80 */
	{
	public:
		hkVector4 translation;   //0x1419D1888

		hkpConvexTranslateShape()
		{
			FUNCTION_PTR(void, __fastcall, ctor, pHkpConvexTranslateShapeCreate, hkpConvexTranslateShape * This, int, int count);
			ctor(this, 0, 1);
		}

		MAKE_DEEP_COPY_FUNC(hkpConvexTranslateShape);
	};

	class hkpConvexVerticesConnectivity /* 0x1419E14D0 */ : public hkReferencedObject /* 1419D14A0 */
	{
	public:
		hkArray<hkUint16> vertexIndices;   //0x1419CE1C0
		hkArray<hkUint8> numVerticesPerFace;   //0x1419CDF10

		hkpConvexVerticesConnectivity()
		{
			FUNCTION_PTR(void, __fastcall, ctor, pHkpConvexVerticesConnectivityCreate, hkpConvexVerticesConnectivity * This, int, int count);
			ctor(this, 0, 1);
		}
	};

	ASSERT_SIZEOF(hkpConvexVerticesConnectivity, 0x38);
	ASSERT_OFFSETOF(hkpConvexVerticesConnectivity, vertexIndices, 0x18);
	ASSERT_OFFSETOF(hkpConvexVerticesConnectivity, numVerticesPerFace, 0x28);

	class hkpConvexVerticesShape /* 0x1419E1900 */ : public hkpConvexShape /* 1419E0C20 */
	{
	public:
		hkVector4 aabbHalfExtents;   //0x1419D1888
		hkVector4 aabbCenter;   //0x1419D1888
		hkArray<hkMatrix3Impl> rotatedVertices;   //0x1419E3920
		hkInt32 numVertices;   //0x1419D2278
		hkBool useSpuBuffer;   //0x1419D0A40
		hkArray<hkVector4> planeEquations;   //0x1419CEBA0
		hkpConvexVerticesConnectivity* connectivity;   //0x1419E3A30

		hkpConvexVerticesShape(float in_radius)
		{
			FUNCTION_PTR(void, __fastcall, ctor, pHkpConvexVerticesShapeCtor, hkpConvexVerticesShape * This, float _radius);
			ctor(this, in_radius);
		}

		hkpConvexVerticesShape()
		{
			FUNCTION_PTR(void, __fastcall, ctor, pHkpConvexVerticesShapeCtor, hkpConvexVerticesShape * This, float _radius);
			ctor(this, ms_DefaultRadius);
		}

		MAKE_DEEP_COPY_FUNC(hkpConvexVerticesShape);
	};

	class hkpCylinderShape /* 0x1419E1A30 */ : public hkpConvexShape /* 1419E0C20 */
	{
	public:
		hkReal cylRadius;   //0x1419D1668
		hkReal cylBaseRadiusFactorForHeightFieldCollisions;   //0x1419D1668
		hkVector4 vertexA;   //0x1419D1888
		hkVector4 vertexB;   //0x1419D1888
		hkVector4 perpendicular1;   //0x1419D1888
		hkVector4 perpendicular2;   //0x1419D1888
	};

	class hkpSphereShape /* 0x1419E1A90 */ : public hkpConvexShape /* 1419E0C20 */
	{
	public:
		hkUint32 pad16[3];   //0x1419D8EB0
	};

	class hkpTriangleShape /* 0x1419E0D30 */ : public hkpConvexShape /* 1419E0C20 */
	{
	public:
		hkUint16 weldingInfo;   //0x1419D21F8
		hkUint8 weldingType;   //0x1419E3030
		hkUint8 isExtruded;   //0x1419D20F8
		hkVector4 vertexA;   //0x1419D1888
		hkVector4 vertexB;   //0x1419D1888
		hkVector4 vertexC;   //0x1419D1888
		hkVector4 extrusion;   //0x1419D1888
	};

	class hkpMoppCode /* 0x1419DFEF0 */ : public hkReferencedObject /* 1419D14A0 */
	{
	public:
		class CodeInfo /* 0x1419DFF50 */
		{
		public:
			hkVector4 offset;   //0x1419D1888
		};

		CodeInfo info;   //0x1419DFF50
		hkArray<hkUint8> data;   //0x1419CDF10
		hkInt8 buildType;   //0x1419E40A0
	};

	class hkMoppBvTreeShapeBase /* 0x1419E00F0 */ : public hkpBvTreeShape /* 1419E2500 */
	{
	public:
		hkpMoppCode* code;   //0x1419E4650
		void* moppData;   //0x1419CC3C0
		hkUint32 moppDataSize;   //0x1419D22E8
		hkVector4 codeInfoCopy;   //0x1419D1888
	};

	class hkpMoppBvTreeShape /* 0x1419E01B0 */ : public hkMoppBvTreeShapeBase /* 1419E00F0 */
	{
	public:
		hkpSingleShapeContainer child;   //0x1419E2120
		int childSize;   //0x1419D99E0

		hkpMoppBvTreeShape(hkpShape* childShape, hkpMoppCode* moppCode)
		{
			FUNCTION_PTR(void, __fastcall, ctor, pHkpMoppBvTreeShapeCtor, hkpMoppBvTreeShape* This, hkpShape * _childShape, hkpMoppCode * _moppCode);
			ctor(this, childShape, moppCode);
		}

		MAKE_DEEP_COPY_FUNC(hkpMoppBvTreeShape);
	};

}
