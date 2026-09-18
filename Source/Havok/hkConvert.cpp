#include <hedgelib/hl_internal.h>
#include <rad/rad_file.h>

#include "hkTypes2010.h"
#include "hkTypes2022.h"

#include "hkConvert.h"

#include "errno.h"


// TODO: This is a LOT of functions. Move to their own CPP file!
void hk2010_2_0::hkpExtendedMeshShape::Finish()
{
	collectionType = COLLECTION_EXTENDED_MESH;

	if (m_trianglesSubparts.m_size > 0)
	{
		for (int i = 0; i < m_trianglesSubparts.m_size; ++i)
		{
			TrianglesSubpart* subpart = &m_trianglesSubparts[i];

			if (!subpart->m_materialIndexStridingType) // 0 = MATERIAL_INDICES_INVALID
				subpart->m_materialIndexStridingType = MATERIAL_INDICES_INT8;
		}
	}

	if (m_shapesSubparts.m_size > 0)
	{
		for (int i = 0; i < m_shapesSubparts.m_size; ++i)
		{
			ShapesSubpart* subPart = &m_shapesSubparts[i];


			// TODO: Subpart sets some flags and we should implement it but for now don't bother.
			//if (subPart)
			//	subPart->Finish();

			if (!subPart->m_materialIndexStridingType)
				subPart->m_materialIndexStridingType = MATERIAL_INDICES_INT8;
		}
	}

	// TODO: Implement maybe???
#if 0
	if (m_trianglesSubparts.m_size == 1)
	{
		TrianglesSubpart* triangleSubparts = m_trianglesSubparts.m_data.get();
		unsigned int i = 0;
		TrianglesSubpart* embeddedTriSubpart = &m_embeddedTrianglesSubpart;
		do
		{
			*(uint32_t*)&embeddedTriSubpart->m_type = *((uint32_t*)&triangleSubparts->m_type + 2 * i);
			embeddedTriSubpart->m_materialIndexBase = *(&triangleSubparts->m_materialIndexBase + 2 * i++);
			embeddedTriSubpart = (TrianglesSubpart*)((char*)embeddedTriSubpart
				+ 8);
		} while (i < 0xE);

		const int m_capacityAndFlags = m_trianglesSubparts.m_capacityAndFlags;
		m_trianglesSubparts.m_size = 0;

		if ((m_capacityAndFlags & 0x80000000) == 0)
			hkContainerHeapAllocator::s_alloc.bufFree(&hkContainerHeapAllocator::s_alloc, m_trianglesSubparts.m_data, 112 * (m_capacityAndFlags & 0x3FFFFFFF));

		m_trianglesSubparts.m_data = &m_embeddedTrianglesSubpart;
		m_trianglesSubparts.m_size = 1;
		*reinterpret_cast<uint32_t*>(m_trianglesSubparts.m_capacityAndFlags) = 0x80000001;
	}
#endif

	m_type = HK_SHAPE_EXTENDED_MESH;

	if (m_cachedNumChildShapes == -1)
		*reinterpret_cast<uint32_t*>(&m_cachedNumChildShapes) = 0x80000000;
}

hk2022::hkpStorageExtendedMeshShape*
hk2022::hkpStorageExtendedMeshShape::DeepCopy(const hk2010_2_0::hkpShape* in_pShape2010)
{
	const auto* oldShape = (hk2010_2_0::hkpStorageExtendedMeshShape*)in_pShape2010;
	auto* myShape = hkConvert::MakeUniquePtr<hkpStorageExtendedMeshShape>(oldShape);

	myShape->userData = oldShape->m_userData;

	myShape->disableWelding = oldShape->disableWelding;
	myShape->collectionType = oldShape->collectionType;

	myShape->aabbHalfExtents = oldShape->m_aabbHalfExtents;
	myShape->aabbCenter = oldShape->m_aabbCenter;
	myShape->numBitsForSubpartIndex = oldShape->m_numBitsForSubpartIndex;
	myShape->weldingType = oldShape->m_weldingType;
	myShape->defaultCollisionFilterInfo = oldShape->m_defaultCollisionFilterInfo;
	myShape->cachedNumChildShapes = oldShape->m_cachedNumChildShapes;
	myShape->triangleRadius = oldShape->m_triangleRadius;

	// FIXME: Duplicate code in for loop. Convert to function.
	{
		TrianglesSubpart mySubpart{};
		auto& oldSubpart = oldShape->m_embeddedTrianglesSubpart;

		mySubpart.setType(oldSubpart.m_type);
		mySubpart.setMaterialIndexStridingType(oldSubpart.m_materialIndexStridingType);
		mySubpart.setNumMaterials(oldSubpart.m_numMaterials);

		// FIND OUT WHAT TO DO WITH THIS ONE? CHECK GENS 2024'S SHAPES ON LOAD.
		// Could very well be unused in the game, but we have to check!
		mySubpart.shapeInfo = 0;

		mySubpart.materialIndexStriding = oldSubpart.m_materialIndexStriding;
		mySubpart.userData = oldSubpart.m_userData;

		mySubpart.numTriangleShapes = oldSubpart.m_numTriangleShapes;
		mySubpart.numVertices = oldSubpart.m_numVertices;
		mySubpart.vertexStriding = oldSubpart.m_vertexStriding;
		mySubpart.triangleOffset = oldSubpart.m_triangleOffset;
		mySubpart.indexStriding = oldSubpart.m_indexStriding;
		mySubpart.stridingType = oldSubpart.m_stridingType;
		mySubpart.flipAlternateTriangles = oldSubpart.m_flipAlternateTriangles;
		mySubpart.extrusion = oldSubpart.m_extrusion;
		mySubpart.transform = oldSubpart.m_transform;

		myShape->embeddedTrianglesSubpart = mySubpart;
	}

	for (auto& oldSubpart : oldShape->m_trianglesSubparts)
	{
		TrianglesSubpart mySubpart {};

		mySubpart.setType(oldSubpart.m_type);
		mySubpart.setMaterialIndexStridingType(oldSubpart.m_materialIndexStridingType);
		mySubpart.setNumMaterials(oldSubpart.m_numMaterials);

		// FIND OUT WHAT TO DO WITH THIS ONE? CHECK GENS 2024'S SHAPES ON LOAD.
		// Could very well be unused in the game, but we have to check!
		mySubpart.shapeInfo = 0;

		mySubpart.materialIndexStriding = oldSubpart.m_materialIndexStriding;
		mySubpart.userData = oldSubpart.m_userData;

		mySubpart.numTriangleShapes = oldSubpart.m_numTriangleShapes;
		mySubpart.numVertices = oldSubpart.m_numVertices;
		mySubpart.vertexStriding = oldSubpart.m_vertexStriding;
		mySubpart.triangleOffset = oldSubpart.m_triangleOffset;
		mySubpart.indexStriding = oldSubpart.m_indexStriding;
		mySubpart.stridingType = oldSubpart.m_stridingType;
		mySubpart.flipAlternateTriangles = oldSubpart.m_flipAlternateTriangles;
		mySubpart.extrusion = oldSubpart.m_extrusion;
		mySubpart.transform = oldSubpart.m_transform;

		myShape->trianglesSubparts.push_back(mySubpart);
	}

	for (auto& oldSubpart : oldShape->m_shapesSubparts)
	{
		ShapesSubpart mySubpart{};

		int i = 0;
		for (auto& childShape : oldSubpart.m_childShapes)
		{
			//if (MessageBoxA(0, hkConvert::s_classNameMap.at(childShape.m_pntr.get()), "ShapeName", MB_OKCANCEL) == IDCANCEL)
			//	exit(-1);

			hkRefPtr myChild = static_cast<hkpConvexShape*>(hkConvert::DeepCopyShape(childShape.m_pntr.get()));
			mySubpart.childShapes.push_back(myChild);
			i++;
		}

		mySubpart.rotation = oldSubpart.m_rotation;
		mySubpart.translation = oldSubpart.m_translation;

		// FIXME: Reused code
		mySubpart.setType(oldSubpart.m_type);
		mySubpart.setMaterialIndexStridingType(oldSubpart.m_materialIndexStridingType);
		mySubpart.setNumMaterials(oldSubpart.m_numMaterials);

		// FIND OUT WHAT TO DO WITH THIS ONE? CHECK GENS 2024'S SHAPES ON LOAD.
		// Could very well be unused in the game, but we have to check!
		mySubpart.shapeInfo = 0;

		mySubpart.materialIndexStriding = oldSubpart.m_materialIndexStriding;
		mySubpart.userData = oldSubpart.m_userData;

		myShape->shapesSubparts.emplace_back(mySubpart);
	}

	for (auto& meshstorage : oldShape->m_meshstorage)
	{
		auto* myStorage = hkConvert::MakeUniquePtr<MeshSubpartStorage>(meshstorage.get());

		myStorage->vertices.copy(meshstorage->m_vertices);
		myStorage->indices8.copy(meshstorage->m_indices8);
		myStorage->indices16.copy(meshstorage->m_indices16);
		myStorage->indices32.copy(meshstorage->m_indices32);
		myStorage->materialIndices.copy(meshstorage->m_materialIndices);
		myStorage->materialIndices16.copy(meshstorage->m_materialIndices16);

		for (const auto& material : meshstorage->m_materials)
		{
			Material myMaterial{};

			myMaterial.friction = material.m_friction;
			myMaterial.restitution = material.m_restitution;
			myMaterial.userData = material.m_userData;

			myStorage->materials.push_back(myMaterial);
		}

		for (const auto& namedMaterial : meshstorage->m_namedMaterials)
		{
			hkpNamedMeshMaterial myNamedMaterial{};

			myNamedMaterial.filterInfo = namedMaterial.filterInfo;
			myNamedMaterial.name = namedMaterial.m_name.c_str();

			myStorage->namedMaterials.push_back(myNamedMaterial);
		}

		myShape->meshstorage.push_back(myStorage);
	}

	for (auto& shapestorage : oldShape->m_shapestorage)
	{
		auto* myStorage = hkConvert::MakeUniquePtr<ShapeSubpartStorage>(shapestorage.get());

		myStorage->materialIndices.copy(shapestorage->m_materialIndices);
		myStorage->materialIndices16.copy(shapestorage->m_materialIndices16);

		for (const auto& material : shapestorage->m_materials)
		{
			Material myMaterial{};

			myMaterial.friction = material.m_friction;
			myMaterial.restitution = material.m_restitution;
			myMaterial.userData = material.m_userData;

			myStorage->materials.push_back(myMaterial);
		}

		myShape->shapestorage.push_back(myStorage);
	}

	myShape->weldingInfo.copy(oldShape->m_weldingInfo);

	FUNCTION_PTR(void, __fastcall, ClassFixBase, 0x14BA62D90, const hkpExtendedMeshShape* This);
	ClassFixBase(myShape);
	FUNCTION_PTR(void, __fastcall, ClassFix, 0x14BC31510, const hkpStorageExtendedMeshShape* This);
	ClassFix(myShape);

	return myShape;
}

hk2022::hkpBoxShape* hk2022::hkpBoxShape::DeepCopy(const hk2010_2_0::hkpShape* in_pShape2010)
{
	const auto* oldShape = (hk2010_2_0::hkpBoxShape*)in_pShape2010;
	auto* myShape = hkConvert::MakeUniquePtr<hkpBoxShape>(oldShape, oldShape->m_halfExtents, oldShape->radius);

	return myShape;
}

void hk2022::hkpConvexTransformShapeBase::DeepCopyChildClass(const hk2010_2_0::hkpConvexTransformShapeBase* in_pShape2010)
{
	//if (MessageBoxA(0,
	//	hkConvert::s_classNameMap.at(in_pShape2010->m_childShape.m_childShape.get()),
	//	"ShapeName",
	//	MB_OKCANCEL) == IDCANCEL)
	//{
	//	exit(-1);
	//}

	childShape.childShape = hkConvert::DeepCopyShape(in_pShape2010->m_childShape.m_childShape.get());
}

hk2022::hkpConvexTransformShape* hk2022::hkpConvexTransformShape::DeepCopy(const hk2010_2_0::hkpShape* in_pShape2010)
{
	const auto* oldShape = (hk2010_2_0::hkpConvexTransformShape*)in_pShape2010;
	auto* myShape = hkConvert::MakeUniquePtr<hkpConvexTransformShape>(oldShape);

	myShape->DeepCopyChildClass(oldShape);

	myShape->type = hk2010_2_0::HK_SHAPE_CONVEX_TRANSFORM;
	memcpy(&myShape->transform, &oldShape->m_transform, sizeof(hkShared::hkQsTransform));
	myShape->extraScale = { 0,0,0,0 };

	return myShape;
}

hk2022::hkpConvexTranslateShape* hk2022::hkpConvexTranslateShape::DeepCopy(const hk2010_2_0::hkpShape* in_pShape2010)
{
	const auto* oldShape = (hk2010_2_0::hkpConvexTranslateShape*)in_pShape2010;
	auto* myShape = hkConvert::MakeUniquePtr<hkpConvexTranslateShape>(oldShape);

	myShape->DeepCopyChildClass(oldShape);

	myShape->type = hk2010_2_0::HK_SHAPE_CONVEX_TRANSLATE;
	myShape->translation = oldShape->m_translation;

	return myShape;
}

hk2022::hkpConvexVerticesShape* hk2022::hkpConvexVerticesShape::DeepCopy(const hk2010_2_0::hkpShape* in_pShape2010)
{
	const auto* oldShape = (hk2010_2_0::hkpConvexVerticesShape*)in_pShape2010;
	auto* myShape = hkConvert::MakeUniquePtr<hkpConvexVerticesShape>(oldShape);

	myShape->radius = oldShape->radius;
	myShape->userData = oldShape->m_userData;
	
	myShape->aabbHalfExtents = oldShape->m_aabbHalfExtents;
	myShape->aabbCenter = oldShape->m_aabbCenter;
	myShape->numVertices = oldShape->m_numVertices;
	myShape->useSpuBuffer = 0;

	auto v0 = oldShape->m_rotatedVertices.m_data.get();
	auto v1 = oldShape->m_rotatedVertices.m_size;
	auto v2 = oldShape->m_rotatedVertices.m_capacityAndFlags;

	auto b0 = oldShape->m_planeEquations.m_data.get();
	auto b1 = oldShape->m_planeEquations.m_size;
	auto b2 = oldShape->m_planeEquations.m_capacityAndFlags;

	for (auto& rotatedVertex : oldShape->m_rotatedVertices)
	{
		hkMatrix3f mat;
		mat.col0 = rotatedVertex.m_x;
		mat.col1 = rotatedVertex.m_y;
		mat.col2 = rotatedVertex.m_z;
		myShape->rotatedVertices.push_back(mat);
	}

	for (auto& planeEquation : oldShape->m_planeEquations)
	{
		myShape->planeEquations.push_back(planeEquation);
	}

	//myShape->rotatedVertices.copy(oldShape->m_rotatedVertices);
	//myShape->planeEquations.copy(oldShape->m_planeEquations);

	auto* myConnectivity = hkConvert::MakeUniquePtr<hkpConvexVerticesConnectivity>(oldShape->m_connectivity.get());
	myShape->connectivity = myConnectivity;

	//if (myConnectivity)
	//{
	//	myConnectivity->numVerticesPerFace.copy(oldShape->m_connectivity->m_numVerticesPerFace);
	//	myConnectivity->vertexIndices.copy(oldShape->m_connectivity->m_vertexIndices);
	//}

	return myShape;
}

hk2022::hkpMoppBvTreeShape* hk2022::hkpMoppBvTreeShape::DeepCopy(const hk2010_2_0::hkpShape* in_pShape2010)
{
	const auto* oldShape = (hk2010_2_0::hkpMoppBvTreeShape*)in_pShape2010;

	auto* childShape = hkConvert::DeepCopyShape(oldShape->m_child.m_childShape.get());
	auto* moppCode   = hkConvert::MakeUniquePtr<hkpMoppCode>(oldShape->m_code.get());

	moppCode->data.copy(oldShape->m_code->m_data);
	moppCode->buildType = oldShape->m_code->m_buildType;
	moppCode->info.offset = oldShape->m_code->m_info.m_offset;

	auto* myShape = hkConvert::MakeUniquePtr<hkpMoppBvTreeShape>(oldShape, childShape, moppCode);

	// Class fix in-place, as it's quite simple/

	myShape->bvTreeType = 0;
	myShape->type = 9; // "MOPP"
	myShape->moppData     = myShape->code->data.m_data;
	myShape->moppDataSize = myShape->code->data.m_size;
	myShape->codeInfoCopy = myShape->code->info.offset;

	return myShape;
}

namespace hkConvert
{
	uint64_t pGenerateRigidBodies = 0x140BFCB80;
	uint64_t pGetHavokClassFromResource = 0x14076C1A0;

	hk2022::hkpPhysicsData* physicsData2022 = nullptr;


	struct hkFullHeader
	{
		hk2010_2_0::hkPackfileHeader base;
		hk2010_2_0::hkPackfileSectionHeader sections[];
	};

	// TODO: Move
	// Source - https://stackoverflow.com/a/16388610
	// Posted by Serhiy, modified by community. See post 'Timeline' for change history
	// Retrieved 2026-09-15, License - CC BY-SA 3.0

	static constexpr unsigned int str2int(const char* str, int h = 0)
	{
		return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
	}

	void InitPhysData_BoxTest()
	{
		// basic rigidbody shape for testing.
		auto shape = new hk2022::hkpBoxShape({ 400,1,400,0 });
		auto rb = new hk2022::hkpRigidBody(hk2022::hkpRigidBodyCinfo(shape));

		auto system = new hk2022::hkpPhysicsSystem();
		system->rigidBodies.emplace_back(rb);

		physicsData2022 = new hk2022::hkpPhysicsData();
		physicsData2022->systems.emplace_back(system);
	}

#if 0
#ifdef _DEBUG
	template<typename T>
	struct OffsetPtr
	{
	private:
		void* _pad {};
		T* m_ptr {};

	public:
		_NODISCARD T* operator->() const noexcept {
			return m_ptr;
		}
	};
#else
	template<typename T>
	struct OffsetPtr
	{
	private:
		T* m_ptr;

	public:
		_NODISCARD T* operator->() const noexcept {
			return m_ptr;
		}
	};
#endif

	union hkPackfileDataArray
	{
		std::vector<char> array;
		OffsetPtr<hkFullHeader> header;
	};
#endif

	static void* CopyPackFile(rad::file_stream& file)
	{
		hk2010_2_0::hkPackfileHeader header;
		file.read_as(header);

		const uint32_t numSections = header.num_sections;

		uint32_t startOffset = 0;
		uint32_t endOffset = 0;

		for (unsigned int i = 0; i < numSections; ++i)
		{
			hk2010_2_0::hkPackfileSectionHeader section;
			file.read_as(section);

			if (section.end_offset < endOffset)
				continue;

			startOffset = section.absolute_data_start;
			endOffset   = section.end_offset;
		}

		uint32_t fileSize = startOffset + endOffset;

		void* fileData = malloc(fileSize);

		file.seek(rad::stream::seek_mode::begin, 0);
		file.read(fileData, fileSize);

		return fileData;
	}

	// vestigial code from an earlier attempt at fixing the data in a copy instead of in-place.
#if 0
	int GetRequiredBufferSize(hkFullHeader* packfileData)
	{
		// edi
		int v3; // ebp
		int v4; // ebx
		// eax
		unsigned int* p_exports_offset; // ecx
		unsigned int v7; // edi
		// ecx
		int v10; // [esp+Ch] [ebp-18h]
		int v11; // [esp+10h] [ebp-14h]
		int v12; // [esp+14h] [ebp-10h]
		int numTrackedObjects; // [esp+18h] [ebp-Ch]
		int i; // [esp+20h] [ebp-4h]

		int num_sections = packfileData->base.num_sections;
		v3 = 0;
		v4 = 0;
		numTrackedObjects = 0;

		hk2010_2_0::hkPackfileSectionHeader* sections = num_sections > 0
			? packfileData->sections
			: nullptr;

		int sectionIndex = 0;
		v10 = 0;
		v12 = 0;
		v11 = 0;

		if (num_sections >= 2)
		{
			p_exports_offset = &sections[1].exports_offset;
			v7 = ((unsigned int)(num_sections - 2) >> 1) + 1;
			i = 2 * v7;
			do
			{
				v3 += (*(p_exports_offset - 12) - *(p_exports_offset - 13)) / 12;
				v12 += *(p_exports_offset - 10) + *(p_exports_offset - 15) - *(p_exports_offset - 12);
				v10 += (*p_exports_offset - *(p_exports_offset - 1)) / 12;
				v11 += p_exports_offset[2] + *(p_exports_offset - 3) - *p_exports_offset;
				p_exports_offset += 24;
				--v7;
			} while (v7);

			sectionIndex = i;
		}
		if (sectionIndex < num_sections)
		{
			hk2010_2_0::hkPackfileSectionHeader* section = &sections[sectionIndex];
			v4 = section->imports_offset + section->end_offset + section->local_fixups_offset - section->imports_offset - section->exports_offset;
			numTrackedObjects = (section->exports_offset - section->virtual_fixups_offset) / 12;
		}
		return v4 + v12 + v11 + 8 * (numTrackedObjects + v3 + v10 + 2 * (3 * num_sections + 3));
	}


	static void CopySection(
		        const hk2010_2_0::hkPackfileSectionHeader *inSection,
		        hk2010_2_0::hkPackfileSectionHeader *outSection,
		        int outCurOffset,
		        char *packfileData,
		        char *outBuffer)
	{
		memcpy(outSection->section_tag, inSection->section_tag, sizeof(inSection->section_tag));
		outSection->absolute_data_start = outCurOffset;
		outSection->local_fixups_offset = inSection->local_fixups_offset;
		outSection->global_fixups_offset = inSection->local_fixups_offset;
		outSection->virtual_fixups_offset = inSection->local_fixups_offset;
		outSection->exports_offset = inSection->local_fixups_offset;
		outSection->imports_offset = inSection->imports_offset + inSection->local_fixups_offset - inSection->exports_offset;
		outSection->end_offset = inSection->end_offset + inSection->local_fixups_offset - inSection->exports_offset;

		char* dataPtr = &packfileData[inSection->absolute_data_start];
		memcpy(&outBuffer[outCurOffset], dataPtr, inSection->local_fixups_offset);
		memcpy(
			&outBuffer[outCurOffset + outSection->exports_offset],
			&dataPtr[inSection->exports_offset],
			inSection->imports_offset - inSection->exports_offset);
		memcpy(
			&outBuffer[outCurOffset + outSection->imports_offset],
			&dataPtr[inSection->imports_offset],
			inSection->end_offset - inSection->imports_offset);
	}
#endif

	static void ApplyLocalFixups(
		const hk2010_2_0::hkPackfileSectionHeader* inSection,
		      hk2010_2_0::hkPackfileSectionHeader* outSection, // only used for hack array access lol
		char* outBuffer,
		int   outBufferOffset)
	{
		const int fixupIndex = (inSection->global_fixups_offset - inSection->local_fixups_offset) / 4;

		if (fixupIndex <= 0)
			return;

		      int32_t* bufferPtr = (int32_t*)&outBuffer[outBufferOffset];
		const int32_t* fixupData = (int32_t*)&outSection->section_tag[inSection->local_fixups_offset + inSection->absolute_data_start];

		for (int i = 0; i < fixupIndex; i += 2)
		{
			const int currentPtr = fixupData[i];

			if (currentPtr == -1)
				continue;

			auto* pointer32 = reinterpret_cast<hl::off32<int>*>(&bufferPtr[currentPtr / 4]);
			pointer32->set(&bufferPtr[fixupData[i + 1] / 4]);
		}
	}

	static void ApplyGlobalFixups(
		const hk2010_2_0::hkPackfileSectionHeader* inSection,
		      hk2010_2_0::hkPackfileSectionHeader* outSection,
		char* packfileData,
		char* outBuffer,
		hk2010_2_0::hkPackfileSectionHeader* outSections)
	{
		const int fixupIndex = (inSection->virtual_fixups_offset - inSection->global_fixups_offset) / 4;
		char* bufferPtr = &outBuffer[outSection->absolute_data_start];

		if (fixupIndex <= 0)
			return;

		const int32_t* fixupData = (int32_t*)&packfileData[inSection->global_fixups_offset + 8 + inSection->absolute_data_start];

		for (int i = 0; i < fixupIndex; i += 3, fixupData += 3)
		{
			const int currentPtr = *(fixupData - 2);

			if (currentPtr == -1)
				continue;

			hk2010_2_0::hkPackfileSectionHeader* sectionHeader = &outSections[*(fixupData - 1)];

			char* outData = sectionHeader->local_fixups_offset
			              ? &outBuffer[sectionHeader->absolute_data_start + *fixupData]
			              : nullptr;

			auto* pointer32 = reinterpret_cast<hl::off32<char>*>(&bufferPtr[currentPtr]);
			pointer32->set(outData);

			//*(int*)&bufferPtr[currentPtr] = outData;
		}
	}

	struct TrackedObjectsArray_Element
	{
		void* m_typeInfo;
		int m_offset;
	};

	// Utility define to reduce redundancy and ensure we're using the class names directly.
#define HANDLE_SHAPE_CASE(x) \
	case str2int(#x): \
	{ \
		shape = hk2022::##x::DeepCopy(pShape2010);\
		break;\
	} \

	static hk2022::hkpShape* DeepCopyShape(const hk2010_2_0::hkpShape* pShape2010)
	{
		if (!pShape2010)
			return nullptr;

		hk2022::hkpShape* shape;

		if (uniquePointerMap.contains(pShape2010))
			return (hk2022::hkpShape*)uniquePointerMap.at(pShape2010);

		switch (str2int(s_classNameMap.at(pShape2010)))
		{
		default:
			return nullptr;

			HANDLE_SHAPE_CASE(hkpStorageExtendedMeshShape)
			HANDLE_SHAPE_CASE(hkpMoppBvTreeShape)
			HANDLE_SHAPE_CASE(hkpConvexTransformShape)
			HANDLE_SHAPE_CASE(hkpConvexTranslateShape)
			HANDLE_SHAPE_CASE(hkpBoxShape)
			HANDLE_SHAPE_CASE(hkpConvexVerticesShape)
		}

		return shape;
	}

	static hk2022::hkpRigidBody* DeepCopyRigidBody(hk2010_2_0::hkpRigidBody* in_pBody)
	{
		hk2022::hkpShape* shape = DeepCopyShape(in_pBody->m_collidable.m_shape.get());
		if (!shape)
			return nullptr;

		return new hk2022::hkpRigidBody(hk2022::hkpRigidBodyCinfo(shape));
	}

	void ApplyVirtualFixups(
		const hk2010_2_0::hkPackfileSectionHeader* inSection,
		hk2010_2_0::hkPackfileSectionHeader* outSection,
		char* packfileData,
		uint32_t* outBuffer,
		hk2010_2_0::hkPackfileSectionHeader* outSections,
		std::vector<TrackedObjectsArray_Element>* elementList = nullptr)
	{
		int m_virtual_fixups_offset = inSection->virtual_fixups_offset;
		char* contentsPointer = (char*)outBuffer + outBuffer[1];
		char* fixupData = &packfileData[m_virtual_fixups_offset + inSection->absolute_data_start];

		char* sectionData = (char*)outBuffer + outSection->absolute_data_start;

		if ((inSection->exports_offset - m_virtual_fixups_offset) / 4 <= 0)
			return;

		int offsetA = 8;
		uint32_t* fixupOfsetted = (uint32_t*)(fixupData + 8);
		int fixupDataSub = -8 - (uint32_t)fixupData;

		for (int i = 0; i < (inSection->exports_offset - inSection->virtual_fixups_offset) / 4; i += 3, offsetA += 12, fixupOfsetted += 3)
		{
			int objectIndex = *(fixupOfsetted - 2);

			if (objectIndex == -1)
				continue;

			int classNameIndex = *(fixupOfsetted - 1);
			const char* className = outSections[classNameIndex].local_fixups_offset
			                        ? (char*)outBuffer + *fixupOfsetted + outSections[classNameIndex].absolute_data_start
			                        : nullptr;

			// We don't have this functionality, but we also don't need it really. We'll reimplement what we need instead.
			//void* foundObject = finishRegistry->finishLoadedObject(finishRegistry, &sectionData[objectIndex], className);// commonly loads 0x00588A60

			char* objAddress = &sectionData[objectIndex];

			//auto* pNameID = (uint32_t*)objAddress;
			//*pNameID = str2int(className);

			//FixShapeClass(objAddress, str2int(className));
			s_classNameMap.emplace(objAddress, className);

			// stub for now
			void* foundObject = nullptr;

			if (!foundObject)
				continue;

			if (elementList)
			{
				int typeOffset = objectIndex + outSection->absolute_data_start;
				elementList->push_back({foundObject, typeOffset});

				if (objAddress == contentsPointer)
					outBuffer[8] = (uint32_t)foundObject;
			}
			else
			{
				int fixupOffsetStart = (int)outBuffer + outSection->virtual_fixups_offset + outSection->absolute_data_start;
				*(uint32_t*)((char*)fixupOfsetted + fixupDataSub + fixupOffsetStart + 4) = 0;
				*(uint32_t*)(offsetA + fixupOffsetStart) = (uint32_t)foundObject;
			}
		}
	}

	static hk2010_2_0::hkRootLevelContainer* FixPackfileAndGetContainer(hkFullHeader* packfileData)
	{
		const int numSections = packfileData->base.num_sections;

		if (numSections <= 0)
			return nullptr;

		if ((packfileData->base.flags & 1) != 0)
			return nullptr;

		packfileData->base.flags |= 1;

		uint32_t contentOffset = -1;

		for (int i = 0; i < packfileData->base.num_sections; ++i)
		{
			hk2010_2_0::hkPackfileSectionHeader* section = &packfileData->sections[i];

			// Gives us the __data__ section directly.
			if (i == packfileData->base.contents_section_index)
				contentOffset = packfileData->base.contents_section_offset + section->absolute_data_start;

			ApplyLocalFixups(section, (hk2010_2_0::hkPackfileSectionHeader*)&packfileData->base, (char*)packfileData, section->absolute_data_start);
		}

		for (int i = 0; i < packfileData->base.num_sections; ++i)
		{
			hk2010_2_0::hkPackfileSectionHeader* section = &packfileData->sections[i];

			if (section->virtual_fixups_offset == section->global_fixups_offset)
				continue;

			// FIXME: The arguments here are quite redundant. Clean this up!
			ApplyGlobalFixups(section, section, (char*)packfileData, (char*)packfileData, packfileData->sections);
		}

		for (int i = 0; i < packfileData->base.num_sections; ++i)
		{
			hk2010_2_0::hkPackfileSectionHeader* section = &packfileData->sections[i];

			if (section->exports_offset == section->virtual_fixups_offset)
				continue;

			ApplyVirtualFixups(section, section, (char*)packfileData, (uint32_t*)packfileData, packfileData->sections);
		}

		// TODO: we MIGHT need to account for whenever the top level class is actually NOT the root level container. Put a pin in this.
		return reinterpret_cast<hk2010_2_0::hkRootLevelContainer*>((char*)packfileData + contentOffset);
	}

	void InitPhysData()
	{
		rad::file_stream file = rad::file_stream(
			R"(C:\Users\DanL\Desktop\!Active\_PersonalWork\RND\#ghz200\ghz200_col.phy.hkx)",
			rad::file_stream::OPEN_MODE_READ_ONLY);

		hkFullHeader* packFile = (hkFullHeader*)CopyPackFile(file);

		file.close();

		hk2010_2_0::hkRootLevelContainer* rootLevelContainer = FixPackfileAndGetContainer(packFile);

		hk2010_2_0::hkpPhysicsData* physicsData2010 = nullptr;

		for (int i = 0; i < rootLevelContainer->m_namedVariants.m_size; ++i)
		{
			auto& namedVariant = rootLevelContainer->m_namedVariants.m_data[i];

			// I could be doing it wrong, or it's an issue with converting the hkStringPtr
			// but strstr would fail despite the strings being the same.
			// Look into this if you wish, but for now string_view gets the job done.
			// And, yes, string comparisons is how Havok finds classes too.
			if (std::string_view(namedVariant.m_className.c_str()) != "hkpPhysicsData")
				continue;

			physicsData2010 = static_cast<hk2010_2_0::hkpPhysicsData*>(namedVariant.m_variant.m_pntr.get());
		}

		if (!physicsData2010)
			return;

		// Now we perform our deep copy.

#if 1
		physicsData2022 = new hk2022::hkpPhysicsData();

		for (auto& system2010 : physicsData2010->m_systems)
		{
			auto system2022 = new hk2022::hkpPhysicsSystem();

			for (auto& rigidBody2010 : system2010->m_rigidBodies)
			{
				auto rb = DeepCopyRigidBody(rigidBody2010.get());

				// Don't add types we haven't implemented yet.
				if (!rb)
					continue;

				rb->name = rigidBody2010->m_name.get();
				system2022->rigidBodies.emplace_back(rb);
			}

			physicsData2022->systems.emplace_back(system2022);
		}

#else
		for (int i = 0; i < physicsData2010->m_systems.size; ++i)
		{
			hk2010_2_0::hkpPhysicsSystem* system2010 = physicsData2010->m_systems.m_data[i].get();

			std::stringstream stream;

			for (int j = 0; j < system2010->m_rigidBodies.size; ++j)
			{
				//auto rb = DeepCopyRigidBody(system2010->m_rigidBodies.m_data[j].get());

				stream << system2010->m_rigidBodies.m_data[j]->m_collidable.m_shape->m_type << "\n";
			}

			//MessageBoxA(0, stream.str().c_str(), 0, 0);
		}
#endif

		// TODO: re-assess using a list/vector/scoped_ptr so we aren't doing manual mem management.
		free(packFile);

		uniquePointerMap.clear();

		// DEBUG ONLY: REMOVE!!!
		//exit(0);

#if 0
		auto shape = new hk2022::hkpBoxShape({ 1600,	1,1600,0 });
		auto rb = new hk2022::hkpRigidBody(hk2022::hkpRigidBodyCinfo(shape));

		auto khSystem = new hk2022::hkpPhysicsSystem();
		khSystem->rigidBodies.emplace_back(rb);

		physicsData2022 = new hk2022::hkpPhysicsData();
		physicsData2022->systems.emplace_back(khSystem);
#endif
	}

	bool overridePhysicsData = false;
	HOOK(void, __fastcall, GenerateRigidBodies, pGenerateRigidBodies, const char** a1, char* a2, unsigned int a3, void* a4)
	{
		if (std::string_view(*a1) != "ghz200_col")
		{
			originalGenerateRigidBodies(a1, a2, a3, a4);
			return;
		}

		if (!physicsData2022)
			InitPhysData();

		overridePhysicsData = true;
		originalGenerateRigidBodies(a1, a2, a3, a4);
	}

	HOOK(void*, __fastcall, GetHavokClassFromResource, pGetHavokClassFromResource, void* This, const char* name, size_t a3)
	{
		if (!(overridePhysicsData && physicsData2022 && std::string_view(name) == "hkpPhysicsData"))
			return originalGetHavokClassFromResource(This, name, a3);

		overridePhysicsData = false;
		return physicsData2022;
	}

    void Init()
    {
#if 1
        // Backbuffer patch allowing triple/quad buffering
        // TODO: Apply to modloader

		//InitPhysData();

        WRITE_MEMORY(0x1402DDADA, uint8_t, 0xB8, 0x1C, 0x00, 0x00, 0x00, 0x90, 0x90);
        WRITE_MEMORY(0x1402DDB0A, uint8_t, 0x48, 0xC7, 0x45, 0x1C, 0x01, 0x00, 0x00);
        WRITE_MEMORY(0x1402DDB12, uint8_t, 0x90, 0x90, 0x90, 0x90, 0xEB);

        // Stage debug view
         //TODO: Sigscan
        uintptr_t lockFlags = 0x14123FB03;  // Incorrectly set in SXSG
        uintptr_t assertJump = 0x1402CBBB2; // Assert gets tripped regardless, might be code stripped from 2011. Skip over.
        uintptr_t IsCollisionRender = 0x0141F5241E;

        WRITE_MEMORY(lockFlags, uint32_t, 0x2000);
        WRITE_MEMORY(assertJump, uint8_t, 0xEB);
        WRITE_MEMORY(IsCollisionRender, bool, true);
#endif

        INSTALL_HOOK(GenerateRigidBodies);
        INSTALL_HOOK(GetHavokClassFromResource);
    }
}
