#include <hedgelib/hl_internal.h>

#include "hkTypes2010.h"
#include "hkTypes2022.h"

#include "hkConvert.h"

void hk2022::hkpConvexTransformShapeBase::DeepCopyChildClass(const hk2010_2_0::hkpConvexTransformShapeBase* in_pShape2010)
{
	childShape.childShape = hkConvert::DeepCopyShape(in_pShape2010->m_childShape.m_childShape.get());
}

void hk2022::hkpExtendedMeshShape::Subpart::Copy(const hk2010_2_0::hkpExtendedMeshShape::Subpart& oldSubpart)
{
	setType(oldSubpart.m_type);
	setMaterialIndexStridingType(oldSubpart.m_materialIndexStridingType);
	setNumMaterials(oldSubpart.m_numMaterials);

	// FIND OUT WHAT TO DO WITH THIS ONE? CHECK GENS 2024'S SHAPES ON LOAD.
	// Could very well be unused in the game, but we have to check!
	shapeInfo = 0;

	materialIndexStriding = oldSubpart.m_materialIndexStriding;
	userData = oldSubpart.m_userData;
}

void hk2022::hkpExtendedMeshShape::ShapesSubpart::Copy(const hk2010_2_0::hkpExtendedMeshShape::ShapesSubpart& oldSubpart)
{
	Subpart::Copy(oldSubpart);

	// Skyth: This is where I'm currently getting a crash.
	// There's a few other examples at the end of the file you can enable, I labeled them with #define's.
	//
	// I added these ints to check at what index the corruption seems to be happening.
	static int dbgCountA = 0;
	static int dbgCountB = 0;
	for (auto& childShape : oldSubpart.m_childShapes)
	{
		//if (MessageBoxA(0, hkConvert::s_classNameMap.at(childShape.m_pntr.get()), "ShapeName", MB_OKCANCEL) == IDCANCEL)
		//	exit(-1);

		if (dbgCountA == 2 && dbgCountB == 1)
			__debugbreak();

		auto* myChild = static_cast<hkpConvexShape*>(hkConvert::DeepCopyShape(childShape.m_pntr.get()));

		if (myChild)
			myChild->addReference();

		// Crash should happen right here.
		childShapes.push_back(myChild);
		dbgCountB++;
	}

	dbgCountA++;
	dbgCountB = 0;

	rotation = oldSubpart.m_rotation;
	translation = oldSubpart.m_translation;
}

void hk2022::hkpExtendedMeshShape::TrianglesSubpart::Copy(const hk2010_2_0::hkpExtendedMeshShape::TrianglesSubpart& oldSubpart)
{
	Subpart::Copy(oldSubpart);

	numTriangleShapes = oldSubpart.m_numTriangleShapes;
	numVertices = oldSubpart.m_numVertices;
	vertexStriding = oldSubpart.m_vertexStriding;
	triangleOffset = oldSubpart.m_triangleOffset;
	indexStriding = oldSubpart.m_indexStriding;
	stridingType = oldSubpart.m_stridingType;
	flipAlternateTriangles = oldSubpart.m_flipAlternateTriangles;
	extrusion = oldSubpart.m_extrusion;
	transform = oldSubpart.m_transform;
}

hk2022::hkpStorageExtendedMeshShape*
hk2022::hkpStorageExtendedMeshShape::DeepCopy(const hk2010_2_0::hkpShape* in_pShape2010)
{
	const auto* oldShape = (hk2010_2_0::hkpStorageExtendedMeshShape*)in_pShape2010;
	auto* myShape = hkConvert::GetOrMakeDuplicatePtr<hkpStorageExtendedMeshShape>(oldShape);

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

	myShape->embeddedTrianglesSubpart = TrianglesSubpart(oldShape->m_embeddedTrianglesSubpart);

	for (auto& oldSubpart : oldShape->m_trianglesSubparts)
	{
		myShape->trianglesSubparts.push_back(TrianglesSubpart(oldSubpart));
	}

	for (auto& oldSubpart : oldShape->m_shapesSubparts)
	{
		myShape->shapesSubparts.push_back(ShapesSubpart(oldSubpart));
	}

	for (auto& meshstorage : oldShape->m_meshstorage)
	{
		auto* myStorage = hkConvert::GetOrMakeDuplicatePtr<MeshSubpartStorage>(meshstorage.get());

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
		auto* myStorage = hkConvert::GetOrMakeDuplicatePtr<ShapeSubpartStorage>(shapestorage.get());

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
	static int dbgInt = 0;

	//if (dbgInt == 12)
	//	__debugbreak();

	// This seemed to consistently crash whenever this function wads hit 13 times.
	// Was true before I remembered to initialize all my fields correctly, but now it seems fine?
	// Something to do with our 13th shape was incorrect for whatever reason..
	// Might have been a stack-allocated struct initializing hkArray with junk data.
	ClassFixBase(myShape);
	FUNCTION_PTR(void, __fastcall, ClassFix, 0x14BC31510, const hkpStorageExtendedMeshShape* This);
	ClassFix(myShape);	

	dbgInt++;

	return myShape;
}

hk2022::hkpConvexTransformShape* hk2022::hkpConvexTransformShape::DeepCopy(const hk2010_2_0::hkpShape* in_pShape2010)
{
	const auto* oldShape = (hk2010_2_0::hkpConvexTransformShape*)in_pShape2010;
	auto* myShape = hkConvert::GetOrMakeDuplicatePtr<hkpConvexTransformShape>(oldShape);

	myShape->DeepCopyChildClass(oldShape);

	myShape->type = hk2010_2_0::HK_SHAPE_CONVEX_TRANSFORM;
	myShape->transform.rotation.set(oldShape->m_transform.rotation);
	myShape->transform.translation = oldShape->m_transform.translation;

	myShape->extraScale = { 0,0,0,0 };

	return myShape;
}

hk2022::hkpConvexTranslateShape* hk2022::hkpConvexTranslateShape::DeepCopy(const hk2010_2_0::hkpShape* in_pShape2010)
{
	const auto* oldShape = (hk2010_2_0::hkpConvexTranslateShape*)in_pShape2010;
	auto* myShape = hkConvert::GetOrMakeDuplicatePtr<hkpConvexTranslateShape>(oldShape);

	myShape->DeepCopyChildClass(oldShape);

	myShape->type = hk2010_2_0::HK_SHAPE_CONVEX_TRANSLATE;
	myShape->translation = oldShape->m_translation;

	return myShape;
}

hk2022::hkpConvexVerticesShape* hk2022::hkpConvexVerticesShape::DeepCopy(const hk2010_2_0::hkpShape* in_pShape2010)
{
	const auto* oldShape = (hk2010_2_0::hkpConvexVerticesShape*)in_pShape2010;
	auto* myShape = hkConvert::GetOrMakeDuplicatePtr<hkpConvexVerticesShape>(oldShape);

	myShape->radius = oldShape->radius;
	myShape->userData = oldShape->m_userData;
	
	myShape->aabbHalfExtents = oldShape->m_aabbHalfExtents;
	myShape->aabbCenter = oldShape->m_aabbCenter;
	myShape->numVertices = oldShape->m_numVertices;
	myShape->useSpuBuffer = 0;


	// I had a crash here too where trying to use the array "copy()" function crashed.
	// It seemed like data was being overwritten incorrectly.
	// Take a look at this whenever you can.
#define DBG_USE_ARRAY_COPY 0

#if DBG_USE_ARRAY_COPY
	// I had this for debugging info just to track what was happening to the old data.
	// If I remember right, the data from hk2010 looked like it was either getting overwritten,
	// or pointing to floating point data which looked corrupted at first.
	// I can't make heads or tails of why that's happening...
	auto v0 = oldShape->m_rotatedVertices.m_data.get();
	auto v1 = oldShape->m_rotatedVertices.m_size;
	auto v2 = oldShape->m_rotatedVertices.m_capacityAndFlags;

	auto b0 = oldShape->m_planeEquations.m_data.get();
	auto b1 = oldShape->m_planeEquations.m_size;
	auto b2 = oldShape->m_planeEquations.m_capacityAndFlags;

	myShape->rotatedVertices.copy(oldShape->m_rotatedVertices);
	myShape->planeEquations.copy(oldShape->m_planeEquations);
#else

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
#endif

	auto* myConnectivity = hkConvert::GetOrMakeDuplicatePtr<hkpConvexVerticesConnectivity>(oldShape->m_connectivity.get());
	myShape->connectivity = myConnectivity;

#define DBG_APPLY_CONNECTIVITY 0

#if DBG_APPLY_CONNECTIVITY
	if (myConnectivity)
	{
		myConnectivity->numVerticesPerFace.copy(oldShape->m_connectivity->m_numVerticesPerFace);
		myConnectivity->vertexIndices.copy(oldShape->m_connectivity->m_vertexIndices);
	}
#endif

	return myShape;
}

hk2022::hkpMoppBvTreeShape* hk2022::hkpMoppBvTreeShape::DeepCopy(const hk2010_2_0::hkpShape* in_pShape2010)
{
	const auto* oldShape = (hk2010_2_0::hkpMoppBvTreeShape*)in_pShape2010;

	auto* childShape = hkConvert::DeepCopyShape(oldShape->m_child.m_childShape.get());
	auto* moppCode   = hkConvert::GetOrMakeDuplicatePtr<hkpMoppCode>(oldShape->m_code.get());

	moppCode->data.copy(oldShape->m_code->m_data);
	moppCode->buildType = oldShape->m_code->m_buildType;
	moppCode->info.offset = oldShape->m_code->m_info.m_offset;

	auto* myShape = hkConvert::GetOrMakeDuplicatePtr<hkpMoppBvTreeShape>(oldShape, childShape, moppCode);

	// Class fix in-place, as it's quite simple/

	myShape->bvTreeType = 0;
	myShape->type = 9; // "MOPP"
	myShape->moppData     = myShape->code->data.m_data;
	myShape->moppDataSize = myShape->code->data.m_size;
	myShape->codeInfoCopy = myShape->code->info.offset;

	return myShape;
}

hk2022::hkpBoxShape* hk2022::hkpBoxShape::DeepCopy(const hk2010_2_0::hkpShape* in_pShape2010)
{
	const auto* oldShape = (hk2010_2_0::hkpBoxShape*)in_pShape2010;
	auto* myShape = hkConvert::GetOrMakeDuplicatePtr<hkpBoxShape>(oldShape, oldShape->m_halfExtents, oldShape->radius);

	return myShape;
}
