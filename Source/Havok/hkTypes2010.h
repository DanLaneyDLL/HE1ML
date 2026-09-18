#pragma once

//#include <vector>
#include "hk2010Base.h"
#include "hk2010Array.h"

namespace hk2010_2_0
{
	typedef void* VoidPtr;

	// Forward declarations
	class hkReferencedObject;

	// --------------------------------------
	// Packfile reading, adapted from LibGens

	enum {
		TYPE_VOID,
		TYPE_BOOL,
		TYPE_CHAR,
		TYPE_INT8,
		TYPE_UINT8,
		TYPE_INT16,
		TYPE_UINT16,
		TYPE_INT32,
		TYPE_UINT32,
		TYPE_INT64,
		TYPE_UINT64,
		TYPE_REAL,
		TYPE_VECTOR4,
		TYPE_QUATERNION,
		TYPE_MATRIX3,
		TYPE_ROTATION,
		TYPE_QSTRANSFORM,
		TYPE_MATRIX4,
		TYPE_TRANSFORM,
		TYPE_ZERO,
		TYPE_POINTER,
		TYPE_FUNCTION_POINTER,
		TYPE_ARRAY,
		TYPE_INPLACE_ARRAY,
		TYPE_ENUM,
		TYPE_STRUCT,
		TYPE_SIMPLE_ARRAY,
		TYPE_HOMOGENEOUS_ARRAY,
		TYPE_VARIANT,
		TYPE_CSTRING,
		TYPE_ULONG,
		TYPE_FLAGS,
		TYPE_HALF,
		TYPE_STRING_POINTER
	};

	class hkClassName {
	public:
		unsigned int tag;
		std::string name;

		hkClassName() {}
	};

	class hkTypeMember {
	public:
		unsigned char tag[2];
		unsigned short array_size;
		unsigned short struct_type;
		unsigned short offset;
		unsigned int structure_address;
		std::string name;
		std::string structure;

		hkTypeMember() {}
	};

	class hkPackfileEnum {
	public:
		unsigned int id;
		std::string name;
	};

	class hkPackfileType {
	public:
		unsigned int object_size;
		std::string name;
		std::string class_name;
		unsigned int described_version;
		unsigned int num_implemented_interfaces;
		unsigned int declared_enums;
		unsigned int address;

		std::vector<hkTypeMember> members;
		std::vector<hkPackfileEnum> enums;
		std::vector<std::vector<hkPackfileEnum>> sub_enums;
		std::vector<std::string> sub_enum_names;

		hkPackfileType* parent;
		unsigned int parent_address;

		void reset() {
			members.clear();
			enums.clear();
			sub_enums.clear();
			sub_enum_names.clear();
			name = "";

			parent = NULL;
			parent_address = 0;
		}
	};

	class hkLink {
	public:
		unsigned int type;
		unsigned int address_1;
		unsigned int address_2;
		hkPackfileType* type_parent;
		hkPackfileType* type_node;
	};

	class hkPackfilePointer {
	public:
		unsigned abs_address;
		unsigned target_address;
	};

	struct hkPackfileHeader {
		int magic[2];
		int user_tag;
		int file_version;
		unsigned char layout_rules[4];
		int num_sections;
		int contents_section_index;
		int contents_section_offset;
		int contents_class_name_section_index;
		int contents_class_name_section_offset;
		char contents_version[16];
		int flags;
		int pad[1];
	};

	struct hkPackfileSectionHeader {
		char section_tag[19];
		char null_byte;
		unsigned int absolute_data_start;
		unsigned int local_fixups_offset;
		unsigned int global_fixups_offset;
		unsigned int virtual_fixups_offset;
		unsigned int exports_offset;
		unsigned int imports_offset;
		unsigned int end_offset;
	};

	// ---------------------------------------------------------
	// Actual havok types

	class hkReferencedObject
	{
	protected:
		uint32_t vfTable {};
		uint16_t m_memSizeAndFlags;
		uint16_t m_referenceCount;
	};

	template <typename T>
	class hkSmallArray
	{
	public:
		hl::off32<T> m_data;
		uint16_t m_size;
		uint16_t m_capacityAndFlags;
	};


	struct hkStringPtr
	{
		inline const char* c_str() const
		{
			// Due to the quirks of hl::off32 and havok strings using the "owned flag," we have to be a bit creative with this.
			// Of course this might not be the most efficient method; we can probably just get the 64bit offset and do our bit math.
			// Still, better safe than sorry for now?

			auto dataPtr = (uint32_t*)&m_stringAndFlag;

			const uint32_t originalvalue = *dataPtr;
			*dataPtr &= ~OWNED_FLAG;

			const char* result = m_stringAndFlag.get();
			*dataPtr = originalvalue;

			return result;
		}

		inline operator const char* () const
		{
			return c_str();
		}

	private:
		enum StringFlags
		{
			/// If we own it, we free it.
			OWNED_FLAG = 0x1,
		};

		hl::off32<const char> m_stringAndFlag{};
	};

	template <typename T>
	class hkRefPtr
	{
	public:
		hl::off32<T> m_pntr;

		// We should not be able to copy RefPtrs.
		// This creates problems during iteration and leads to reading invalid/uninitialized data
		// due to the use of hl::off32<>
		hkRefPtr(const hkRefPtr&) = delete;
		hkRefPtr& operator=(const hkRefPtr&) = delete;
	};

	struct hkpRigidBody;
	class hkpConstraintInstance;
	class hkpWorld;

	enum hkpShapeType : __int32
	{
		HK_SHAPE_INVALID = 0x0,
		HK_SHAPE_SPHERE = 0x1,
		HK_FIRST_SHAPE_TYPE = 0x1,
		HK_SHAPE_CYLINDER = 0x2,
		HK_SHAPE_TRIANGLE = 0x3,
		HK_SHAPE_BOX = 0x4,
		HK_SHAPE_CAPSULE = 0x5,
		HK_SHAPE_CONVEX_VERTICES = 0x6,
		HK_SHAPE_COLLECTION = 0x7,
		HK_SHAPE_BV_TREE = 0x8,
		HK_SHAPE_LIST = 0x9,
		HK_SHAPE_MOPP = 0xA,
		HK_SHAPE_CONVEX_TRANSLATE = 0xB,
		HK_SHAPE_CONVEX_TRANSFORM = 0xC,
		HK_SHAPE_SAMPLED_HEIGHT_FIELD = 0xD,
		HK_SHAPE_EXTENDED_MESH = 0xE,
		HK_SHAPE_TRANSFORM = 0xF,
		HK_SHAPE_COMPRESSED_MESH = 0x10,
		HK_SHAPE_COMPOUND = 0x11,
		HK_SHAPE_MAX_ID_SPU = 0x12,
		HK_SHAPE_CONVEX = 0x13,
		HK_SHAPE_MOPP_EMBEDDED = 0x14,
		HK_SHAPE_CONVEX_PIECE = 0x15,
		HK_SHAPE_MULTI_SPHERE = 0x16,
		HK_SHAPE_CONVEX_LIST = 0x17,
		HK_SHAPE_TRIANGLE_COLLECTION = 0x18,
		HK_SHAPE_MULTI_RAY = 0x19,
		HK_SHAPE_HEIGHT_FIELD = 0x1A,
		HK_SHAPE_SPHERE_REP = 0x1B,
		HK_SHAPE_BV = 0x1C,
		HK_SHAPE_PLANE = 0x1D,
		HK_SHAPE_PHANTOM_CALLBACK = 0x1E,
		HK_SHAPE_USER0 = 0x1F,
		HK_SHAPE_USER1 = 0x20,
		HK_SHAPE_USER2 = 0x21,
		HK_SHAPE_MAX_ID = 0x22,
		HK_SHAPE_ALL = -0x1,
	};

	struct hkpShape : hkReferencedObject
	{
		unsigned int m_userData;
		union
		{
			hkpShapeType m_type;
			uint32_t m_typeRaw;
		};
	};

	struct hkpCdBody
	{
		const hl::off32<hkpShape> m_shape;
		unsigned int m_shapeKey;
		const hl::off32<void> m_motion;
		const hl::off32<hkpCdBody> m_parent;
	};

	struct hkpBroadPhaseHandle
	{
		unsigned int m_id;
	};

	struct hkpTypedBroadPhaseHandle : hkpBroadPhaseHandle
	{
		char m_type;
		char m_ownerOffset;
		char m_objectQualityType;
		unsigned int m_collisionFilterInfo;
	};

	struct hkAabbUint32
	{
		unsigned int m_min[3];
		uint8_t m_expansionMin[3];
		uint8_t m_expansionShift;
		unsigned int m_max[3];
		uint8_t m_expansionMax[3];
		uint8_t m_shapeKeyByte;
	};
	ASSERT_SIZEOF(hkAabbUint32, 0x20);

	struct hkpCollidable : hkpCdBody
	{
		struct BoundingVolumeData
		{
			unsigned int m_min[3];
			uint8_t m_expansionMin[3];
			uint8_t m_expansionShift;
			unsigned int m_max[3];
			uint8_t m_expansionMax[3];
			uint8_t m_padding;
			uint16_t m_numChildShapeAabbs;
			uint16_t m_capacityChildShapeAabbs;
			hl::off32<hkAabbUint32> m_childShapeAabbs;
			hl::off32<unsigned int> m_childShapeKeys;
		};

		char m_ownerOffset;
		uint8_t m_forceCollideOntoPpu;
		uint16_t m_shapeSizeOnSpu;
		hkpTypedBroadPhaseHandle m_broadPhaseHandle;
		BoundingVolumeData m_boundingVolumeData;
		float m_allowedPenetrationDepth;
	};

	struct hkpLinkedCollidable : hkpCollidable
	{
		hkArray<void*> m_collisionEntries;
	};


	struct hkpPropertyValue
	{
		uint64_t m_data;
	};



	struct hkpProperty
	{
		int m_key;
		int m_alignmentPadding;
		hkpPropertyValue m_value;
	};


	struct hkpWorldObject : hkReferencedObject
	{
		hl::off32<hkpWorld> m_world;
		unsigned int m_userData;
		hkpLinkedCollidable m_collidable;
		char m_multiThreadCheck[12];
		const hl::off32<char> m_name;
		hkArray<hkpProperty> m_properties;
		hl::off32<void> m_treeData;
	};

	struct hkpMaterial
	{
		uint8_t m_responseType;
		uint16_t m_rollingFrictionMultiplier;
		float m_friction;
		float m_restitution;
	};

	struct hkpBreakableBody : hkReferencedObject
	{
	};

	struct hkSweptTransform
	{
		hkVector4f m_centerOfMass0;
		hkVector4f m_centerOfMass1;
		hkQuaternionf m_rotation0;
		hkQuaternionf m_rotation1;
		hkVector4f m_centerOfMassLocal;
	};

	struct hkMotionState
	{
		hkTransform m_Transform;
		hkSweptTransform m_sweptTransform;
		hkVector4f m_deltaAngle;
		float m_objectRadius;
		uint16_t m_linearDamping;
		uint16_t m_angularDamping;
		uint16_t m_timeFactor;
		uint8_t m_maxLinearVelocity;
		uint8_t m_maxAngularVelocity;
		uint8_t m_deactivationClass;
	};

	struct hkpMotion : hkReferencedObject
	{
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

		MotionType m_type;
		uint8_t m_deactivationIntegrateCounter;
		uint16_t m_deactivationNumInactiveFrames[2];
		hkMotionState m_motionState;
		hkVector4f m_inertiaAndMassInv;
		hkVector4f m_linearVelocity;
		hkVector4f m_angularVelocity;
		hkVector4f m_deactivationRefPosition[2];
		unsigned int m_deactivationRefOrientation[2];
		hl::off32<void> m_savedMotion;
		uint16_t m_savedQualityTypeIndex;
		uint16_t m_gravityFactor;
	};

	struct hkpKeyframedRigidMotion : hkpMotion
	{
	};

	struct hkpMaxSizeMotion : hkpKeyframedRigidMotion
	{
	};

	struct hkpEntity : hkpWorldObject
	{
		struct SpuCollisionCallback
		{
			hl::off32<void> m_util;
			uint16_t m_capacity;
			char m_eventFilter;
			char m_userFilter;
		};

		hkpMaterial m_material;
		hl::off32<void> m_limitContactImpulseUtilAndFlag;
		float m_damageMultiplier;
		hl::off32<hkpBreakableBody> m_breakableBody;
		uint32_t m_solverData;
		uint16_t m_storageIndex;
		uint16_t m_contactPointCallbackDelay;
		hkSmallArray<void> m_constraintsMaster;
		hkArray<void*> m_constraintsSlave;
		hkArray<void*> m_constraintRuntime;
		hl::off32<void> m_simulationIsland;
		char m_autoRemoveLevel;
		char m_numShapeKeysInContactPointProperties;
		char m_responseModifierFlags;
		__declspec(align(2)) uint32_t m_uid;
		SpuCollisionCallback m_spuCollisionCallback;
		hkpMaxSizeMotion m_motion;
		hkSmallArray<void> m_contactListeners;
		hkSmallArray<void> m_actions;
		hl::off32<void> m_localFrame;
		hl::off32<void> m_extendedListeners;
		uint32_t m_npData;
	};

	ASSERT_OFFSETOF(hkpEntity, m_uid, 0xD4);

	struct hkpRigidBody : hkpEntity
	{
	};


	class hkpPhysicsSystem : public hkReferencedObject
	{
	public:
		hkArray<hl::off32<hkpRigidBody>> m_rigidBodies;
		hkArray<hl::off32<hkpConstraintInstance>> m_constraints;
		hkArray<void*> m_actions;
		hkArray<void*> m_phantoms;
		hkStringPtr m_name;
		uint32_t m_userData;
		bool m_active;
	};


	class hkpPhysicsData : public hkReferencedObject
	{
	public:
		uint32_t m_worldCinfo {};
		hkArray<hl::off32<hkpPhysicsSystem>> m_systems;
	};


	struct hkRefVariant : hkRefPtr<hkReferencedObject>
	{
	};

	struct hkRootLevelContainer
	{
		struct NamedVariant
		{
			hkStringPtr m_name;
			hkStringPtr m_className;
			hkRefVariant m_variant;
		};

		hkArray<NamedVariant> m_namedVariants;
	};


	class hkpShapeContainer
	{
	public:
		uint32_t containerVtable;
	};

	class hkpShapeCollection : public hkpShape, public hkpShapeContainer
	{
	public:
		enum CollectionType : hkUint8
		{
			COLLECTION_LIST = 0x0,
			COLLECTION_EXTENDED_MESH = 0x1,
			COLLECTION_TRISAMPLED_HEIGHTFIELD = 0x2,
			COLLECTION_SIMPLE_MESH = 0x4,
			COLLECTION_MESH_SHAPE = 0x5,
			COLLECTION_COMPRESSED_MESH = 0x6,
			COLLECTION_MAX = 0x7,
		};

		hkBool disableWelding;
		CollectionType collectionType;
	};

	class hkpMeshMaterial
	{
	public:
		hkUint32 filterInfo;   //0x1419D22E8
	};

	class hkpMeshShape : public hkpShapeCollection
	{
	public:
		class Subpart
		{
		public:
			// Note: opaque types were derived from hk2010_2_0 header information.

			hl::off32<float> vertexBase;   //0x1419CC3C0
			int vertexStriding;   //0x1419D99E0
			int numVertices;   //0x1419D99E0
			hl::off32<VoidPtr> indexBase;   //0x1419CC3C0  -- "A pointer to triples of vertex indices."
			hkInt8 stridingType;   //0x1419E42A0
			hkInt8 materialIndexStridingType;   //0x1419E4440
			int indexStriding;   //0x1419D99E0
			int flipAlternateTriangles;   //0x1419D99E0
			int numTriangles;   //0x1419D99E0
			/// Pointer to a strided array of material index (hkUint8 or hkUint16), one index for each triangle.
			///  - You are limited to a maximum of 256 or 65535 materials per subpart.
			///  - The indices may be stored in an interleaved array by setting m_materialIndexStriding appropriately.
			///  - If you do not want to use materials, simply set this element to HK_NULL
			hl::off32<VoidPtr> materialIndexBase;   //0x1419CC3C0
			int materialIndexStriding;   //0x1419D99E0

			hl::off32<hkpMeshMaterial> materialBase;   //0x1419CC3C0
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

	class hkpSphereRepShape : public hkpShape
	{
	};

	class hkpConvexShape : public hkpSphereRepShape
	{
	protected:
		static constexpr float ms_DefaultRadius = 0.05f;
	public:
		float radius;
	};

	class hkClass;

	struct hkVariant
	{
		hl::off32<VoidPtr> m_object;
		hl::off32<hkClass> m_class;
	};


	class hkCustomAttributes
	{
	public:
		struct Attribute
		{
			const hl::off32<char> m_name;
			hkVariant m_value;
		};

		const hl::off32<Attribute> m_attributes;
		int m_numAttributes;
	};

	class hkClass
	{
	public:
		enum SignatureFlags : hkUint32
		{
			SIGNATURE_LOCAL = 1	// don't include signature of parents
		};

		enum FlagValues : hkUint32
		{
			FLAGS_NONE = 0,
			FLAGS_NOT_SERIALIZABLE = 1
		};
		typedef FlagValues Flags;

		/// Name of this type.
		const hl::off32<char> m_name;

		/// Parent class.
		const hl::off32<hkClass> m_parent;

		/// Size of the live instance.
		int m_objectSize;

		/// Number of interfaces implemented by this class.
		int m_numImplementedInterfaces;

		/// Declared enum members.
		const hl::off32<class hkClassEnum> m_declaredEnums;

		/// Number of enums declared in this class.
		int m_numDeclaredEnums;

		/// Declared members.
		const hl::off32<class hkClassMember> m_declaredMembers;

		/// Number of members declared in this class.
		int m_numDeclaredMembers;

		/// Default values for this class.
		const hl::off32<void> m_defaults; //+nosave

		/// Default values for this class.
		const hl::off32 <hkCustomAttributes> m_attributes; //+serialized(false)

		/// Flag values.
		Flags m_flags;

		/// Version of described object.
		int m_describedVersion;
	};

	class hkpExtendedMeshShape : public hkpShapeCollection
	{
	public:

		/// The striding of mesh indices
		enum IndexStridingType : hkInt8
		{
			INDICES_INVALID, ///< default, will raise assert.

			INDICES_INT8, ///< 8 bit "single byte" striding.

			INDICES_INT16, ///< 16 bit "short" striding.

			INDICES_INT32, ///< 32 bit "int" striding.
			INDICES_MAX_ID
		};

		enum MaterialIndexStridingType : hkInt8
		{
			MATERIAL_INDICES_INVALID,
			MATERIAL_INDICES_INT8,
			MATERIAL_INDICES_INT16,
			MATERIAL_INDICES_MAX_ID
		};

		enum SubpartType : hkInt8
		{
			SUBPART_TRIANGLES,
			SUBPART_SHAPE
		};

		struct Subpart
		{
			SubpartType m_type;

			/// A type defining whether 8 or 16 bits are used to index material.
			MaterialIndexStridingType m_materialIndexStridingType;

			/// The byte offset between two hkMeshMaterials
			hkInt16 m_materialStriding;

			/// A pointer, pointing to a strided array of material index (hkUint8), one index for each triangle.
			///  - You are limited to a maximum of 256 materials per subpart.
			///  - You are not forced to store those indices in a hkUint8 array, with the striding m_materialIndexStriding
			///    parameter you can extract this
			///  - If you do not want to use materials, simply set this element to HK_NULL
			hl::off32<VoidPtr> m_materialIndexBase;

			hkUint16 m_materialIndexStriding;
			hkUint16 m_numMaterials;

			/// The base for the material table, the byte offset between two hkMeshMaterials is defined by
			/// m_materialStriding
			/// Note: On PlayStation(R)3 this must be aligned to a m_materialStriding boundary if you wish to access materials in your own code on SPU.
			hl::off32 <hkpMeshMaterial> m_materialBase;

			/// User data associated with each subpart
			hkUlong m_userData;

		};

		/// A vertices subpart defines a triangle, a triangle list or a triangle strip.
		struct TrianglesSubpart : Subpart
		{
			int	m_numTriangleShapes;
			hl::off32<float> m_vertexBase; // Verts are 3 floats 16aligned. Could just be a pointer to hkVector4f...
			int m_numVertices;
			hl::off32<VoidPtr> m_indexBase; // points to triples of vertex indices.
			hkUint16 m_vertexStriding;
			int m_triangleOffset;
			hkUint16 m_indexStriding;
			IndexStridingType m_stridingType;
			hkInt8 m_flipAlternateTriangles;
			hkVector4 m_extrusion;
			hkQsTransform m_transform;
		};

		struct ShapesSubpart : Subpart
		{
			hkArray<hkRefPtr<hkpConvexShape>> m_childShapes;
			hkQuaternion m_rotation;
			hkVector4 m_translation;

			void Finish();
		};

		void Finish();

		TrianglesSubpart m_embeddedTrianglesSubpart;
		hkVector4 m_aabbHalfExtents;
		hkVector4 m_aabbCenter;
		hl::off32<hkClass> m_materialClass;
		hkInt32 m_numBitsForSubpartIndex;
		hkArray<TrianglesSubpart> m_trianglesSubparts;
		hkArray<ShapesSubpart> m_shapesSubparts;

		hkArray<hkUint16> m_weldingInfo;
		hkUint8 m_weldingType; // default "WELDING_TYPE_NONE"

		hkUint32 m_defaultCollisionFilterInfo;
		mutable hkInt32	 m_cachedNumChildShapes; // default -1
		hkReal m_triangleRadius;
		hkInt32 m_padding; // +nosave
	};

	ASSERT_OFFSETOF(hkpExtendedMeshShape, m_embeddedTrianglesSubpart, 0x20);
	ASSERT_SIZEOF(hkQsTransform, 0x30);

	ASSERT_OFFSETOF(hkpExtendedMeshShape::Subpart, m_type, 0x0);
	ASSERT_OFFSETOF(hkpExtendedMeshShape::Subpart, m_materialIndexStridingType, 0x1);
	ASSERT_OFFSETOF(hkpExtendedMeshShape::Subpart, m_materialStriding, 0x2);
	ASSERT_OFFSETOF(hkpExtendedMeshShape::Subpart, m_materialIndexBase, 0x4);
	ASSERT_OFFSETOF(hkpExtendedMeshShape::Subpart, m_materialIndexStriding, 0x8);
	ASSERT_OFFSETOF(hkpExtendedMeshShape::Subpart, m_numMaterials, 0xA);

	//ASSERT_SIZEOF(hkpExtendedMeshShape::Subpart, 0x14);
	//ASSERT_SIZEOF(hkpExtendedMeshShape::TrianglesSubpart, 0x30);

	//ASSERT_OFFSETOF(hkpExtendedMeshShape, m_aabbHalfExtents, 0x90);

	class hkpNamedMeshMaterial : public hkpMeshMaterial
	{
	public:
		hkStringPtr m_name;
	};

	class hkpStorageExtendedMeshShape : public hkpExtendedMeshShape
	{
	public:
		struct Material : hkpMeshMaterial
		{
			int16_t m_restitution;
			int16_t m_friction;
			hkUlong m_userData;
		};

		struct MeshSubpartStorage : hkReferencedObject
		{
			hkArray<hkVector4> m_vertices;
			hkArray<hkUint8> m_indices8;
			hkArray<hkUint16> m_indices16;
			hkArray<hkUint32> m_indices32;
			hkArray<hkUint8> m_materialIndices; //materialIndices8
			hkArray<Material> m_materials;
			/// When the createRigidBodyFilter exports a mesh with "export triangle selections as materials" checked, then
			/// this array will store the names of the selections.
			hkArray<hkpNamedMeshMaterial> m_namedMaterials;
			hkArray<hkUint16> m_materialIndices16;
		};

		struct ShapeSubpartStorage : hkReferencedObject
		{
			hkArray<hkUint8> m_materialIndices; //materialIndices8
			hkArray<Material> m_materials;
			hkArray<hkUint16> m_materialIndices16;
		};

		friend class hkpShapeDisplayBuilder;
		hkArray<hl::off32<MeshSubpartStorage>>  m_meshstorage;
		hkArray<hl::off32<ShapeSubpartStorage>> m_shapestorage;
	};
	ASSERT_OFFSETOF(hkpStorageExtendedMeshShape, m_meshstorage, 0xF0);
	ASSERT_OFFSETOF(hkpStorageExtendedMeshShape::MeshSubpartStorage, m_vertices, 0x08);

	class hkpMoppCode : public hkReferencedObject
	{
	public:
		enum BuildType : hkUint8
		{
			BUILT_WITH_CHUNK_SUBDIVISION = 0x0,
			BUILD_NOT_SET = 0x2,
		};

		struct CodeInfo
		{
			hkVector4 m_offset;
		};

		CodeInfo m_info;
		hkArray<hkUint8> m_data;
		BuildType m_buildType;
	};

	class hkpBvTreeShape : public hkpShape
	{
	public:

		enum BvTreeType : hkUint8
		{
			BVTREE_MOPP,
			BVTREE_TRISAMPLED_HEIGHTFIELD,
			BVTREE_USER,
			BVTREE_MAX
		};

		BvTreeType m_bvTreeType;
	};


	class hkMoppBvTreeShapeBase : public hkpBvTreeShape
	{
	public:

		const hl::off32<hkpMoppCode> m_code;
		const hl::off32<hkUint8> m_moppData;		//+nosave
		hkUint32					m_moppDataSize; //+nosave
		hkVector4					m_codeInfoCopy; //+nosave
	};

	class hkpSingleShapeContainer : public hkpShapeContainer
	{
	public:
		const hl::off32<hkpShape> m_childShape;
	};

	class hkpConvexTransformShapeBase : public hkpConvexShape
	{
	public:
		hkpSingleShapeContainer m_childShape;
		mutable int m_childShapeSize; // +nosave
	};
	ASSERT_SIZEOF(hkpConvexTransformShapeBase, 0x20);
	ASSERT_SIZEOF(hkpSingleShapeContainer, 0x08);
	ASSERT_OFFSETOF(hkpConvexTransformShapeBase, m_childShape, 0x14);

	class hkpConvexTransformShape : public hkpConvexTransformShapeBase
	{
	public:
		hkTransform m_transform;
	};

	class hkpConvexTranslateShape : public hkpConvexTransformShapeBase
	{
	public:
		hkVector4 m_translation;
	};

	class hkpBoxShape : public hkpConvexShape
	{
	public:
		hkVector4f m_halfExtents;
	};

	/// This class implements a hkpBvTreeShape using MOPP technology.
	class hkpMoppBvTreeShape : public hkMoppBvTreeShapeBase
	{
	public:

		hkpSingleShapeContainer m_child;
		int m_childSize;
	};

	class hkpConvexVerticesConnectivity : public hkReferencedObject
	{
	public:
		hkArray<unsigned short> m_vertexIndices;
		hkArray<unsigned char> m_numVerticesPerFace;
	};

	class hkpConvexVerticesShape : public hkpConvexShape
	{
	public:
		struct FourVectors
		{
			hkVector4 m_x;
			hkVector4 m_y;
			hkVector4 m_z;
		};

		hkVector4 m_aabbHalfExtents;
		hkVector4 m_aabbCenter;
		hkArray<FourVectors> m_rotatedVertices;
		int m_numVertices;
		uint32_t m_externalObject;
		uint32_t m_getFaceNormalsFunc;
		hkArray<hkVector4> m_planeEquations;
		const hl::off32<hkpConvexVerticesConnectivity> m_connectivity;
	};


}
