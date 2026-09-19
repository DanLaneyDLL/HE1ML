#include <hedgelib/hl_internal.h>
#include <rad/rad_file.h>

#include "hkTypes2010.h"
#include "hkTypes2022.h"

#include "hkConvert.h"

#define _DEBUG_STAGE_VIEW


namespace hkConvert
{
	// Source - https://stackoverflow.com/a/16388610
	// Posted by Serhiy, modified by community. See post 'Timeline' for change history
	// Retrieved 2026-09-15, License - CC BY-SA 3.0
	//
	// Not using "MakeStringHash" currently as I want this to be useable in switch statements,
	// which means it must be constexpr.
	// TODO: Move, or use a hash function we already have / turn it into a constexpr function.
	static constexpr unsigned int str2int(const char* str, int h = 0)
	{
		return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
	}

	struct hkPackfile
	{
		hk2010_2_0::hkPackfileHeader header;
		hk2010_2_0::hkPackfileSectionHeader sections[];

		char* rawData()
		{
			return (char*)this;
		}
	};

	static hkPackfile* CopyPackFile(rad::stream& stream)
	{
		hk2010_2_0::hkPackfileHeader header;
		stream.read_as(header);

		const uint32_t numSections = header.num_sections;

		uint32_t startOffset = 0;
		uint32_t endOffset = 0;

		for (unsigned int i = 0; i < numSections; ++i)
		{
			hk2010_2_0::hkPackfileSectionHeader section;
			stream.read_as(section);

			if (section.end_offset < endOffset)
				continue;

			startOffset = section.absolute_data_start;
			endOffset   = section.end_offset;
		}

		uint32_t fileSize = startOffset + endOffset;

		void* fileData = malloc(fileSize);

		stream.seek(rad::stream::seek_mode::begin, 0);
		stream.read(fileData, fileSize);

		return (hkPackfile*)fileData;
	}

	static void ApplyLocalFixups(
		const hk2010_2_0::hkPackfileSectionHeader* inSection,
		hkPackfile* packfile
	)
	{
		const int fixupIndex = (inSection->global_fixups_offset - inSection->local_fixups_offset) / 4;

		if (fixupIndex <= 0)
			return;

		      int32_t* bufferPtr = (int32_t*)&packfile->rawData()[inSection->absolute_data_start];
		const int32_t* fixupData = (int32_t*)&packfile->rawData()[inSection->local_fixups_offset + inSection->absolute_data_start];

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
		const hk2010_2_0::hkPackfileSectionHeader* section,
		hkPackfile* packfile)
	{
		const int fixupIndex = (section->virtual_fixups_offset - section->global_fixups_offset) / 4;
		char* bufferPtr = &packfile->rawData()[section->absolute_data_start];

		if (fixupIndex <= 0)
			return;

		const int32_t* fixupData = (int32_t*)&packfile->rawData()[section->global_fixups_offset + 8 + section->absolute_data_start];

		for (int i = 0; i < fixupIndex; i += 3, fixupData += 3)
		{
			const int currentPtr = *(fixupData - 2);

			if (currentPtr == -1)
				continue;

			hk2010_2_0::hkPackfileSectionHeader* sectionHeader = &packfile->sections[*(fixupData - 1)];

			char* outData = sectionHeader->local_fixups_offset
			              ? &packfile->rawData()[sectionHeader->absolute_data_start + *fixupData]
			              : nullptr;

			auto* pointer32 = reinterpret_cast<hl::off32<char>*>(&bufferPtr[currentPtr]);
			pointer32->set(outData);
		}
	}

	static void ApplyVirtualFixups(
		const hk2010_2_0::hkPackfileSectionHeader* inSection,
		hkPackfile* packfile)
	{
		char* fixupData = &packfile->rawData()[inSection->absolute_data_start + inSection->virtual_fixups_offset];
		char* sectionData = packfile->rawData() + inSection->absolute_data_start;

		const uint32_t endOfSection = (inSection->exports_offset - inSection->virtual_fixups_offset) / 4;

		if (endOfSection <= 0)
			return;

		int32_t* fixupOfsetted = (int32_t*)(fixupData + 8);

		for (unsigned int i = 0; i < endOfSection; i += 3, fixupOfsetted += 3)
		{
			int objectIndex = *(fixupOfsetted - 2);

			if (objectIndex == -1)
				continue;

			int outSectionIndex = *(fixupOfsetted - 1);
			const char* className = packfile->sections[outSectionIndex].local_fixups_offset
				? packfile->rawData() + *fixupOfsetted + packfile->sections[outSectionIndex].absolute_data_start
				: nullptr;

			char* objAddress = &sectionData[objectIndex];
			s_classNameMap.emplace(objAddress, className);
		}
	}

	// Utility define to reduce redundancy and ensure we're using the class names directly.
#define HANDLE_SHAPE_CASE(x) \
	case str2int(#x): \
	{ \
		shape = hk2022::##x::DeepCopy(pShape2010);\
		break;\
	} \

	hk2022::hkpShape* DeepCopyShape(const hk2010_2_0::hkpShape* pShape2010)
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

	static hk2010_2_0::hkRootLevelContainer* FixPackfileAndGetContainer(hkPackfile* packfileData)
	{
		const int numSections = packfileData->header.num_sections;

		if (numSections <= 0)
			return nullptr;

		if ((packfileData->header.flags & 1) != 0)
			return nullptr;

		packfileData->header.flags |= 1;

		uint32_t contentOffset = -1;

		for (int i = 0; i < packfileData->header.num_sections; ++i)
		{
			hk2010_2_0::hkPackfileSectionHeader* section = &packfileData->sections[i];

			// Gives us the __data__ section directly.
			if (i == packfileData->header.contents_section_index)
				contentOffset = packfileData->header.contents_section_offset + section->absolute_data_start;

			ApplyLocalFixups(section, packfileData);
		}

		for (int i = 0; i < packfileData->header.num_sections; ++i)
		{
			hk2010_2_0::hkPackfileSectionHeader* section = &packfileData->sections[i];

			if (section->virtual_fixups_offset == section->global_fixups_offset)
				continue;

			ApplyGlobalFixups(section, packfileData);
		}

		for (int i = 0; i < packfileData->header.num_sections; ++i)
		{
			hk2010_2_0::hkPackfileSectionHeader* section = &packfileData->sections[i];

			if (section->exports_offset == section->virtual_fixups_offset)
				continue;

			ApplyVirtualFixups(section, packfileData);
		}

		// TODO: we MIGHT need to account for whenever the top level class is actually NOT the root level container. Put a pin in this.
		return reinterpret_cast<hk2010_2_0::hkRootLevelContainer*>((char*)packfileData + contentOffset);
	}

	static hk2022::hkpPhysicsData* CreatePhysicsDataFromPackfile(hkPackfile* packFile)
	{
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
			return nullptr;

		auto* physicsData2022 = new hk2022::hkpPhysicsData();

		for (auto& system2010 : physicsData2010->m_systems)
		{
			auto* system2022 = new hk2022::hkpPhysicsSystem();

			for (auto& rigidBody2010 : system2010->m_rigidBodies)
			{
				auto* rb = DeepCopyRigidBody(rigidBody2010.get());

				// Don't add types we haven't implemented yet.
				if (!rb)
					continue;

				rb->name = rigidBody2010->m_name.get();
				system2022->rigidBodies.emplace_back(rb);
			}

			physicsData2022->systems.emplace_back(system2022);
		}

		return physicsData2022;
	}

	void InitPhysData()
	{
		rad::file_stream file = rad::file_stream(
			R"(C:\Users\DanL\Desktop\!Active\_PersonalWork\RND\#ghz200\ghz200_col.phy.hkx)",
			rad::file_stream::OPEN_MODE_READ_ONLY);

		hkPackfile* packFile = CopyPackFile(file);

		file.close();

		s_PhysicsData2022 = CreatePhysicsDataFromPackfile(packFile);

		// TODO: re-assess using a list/vector/scoped_ptr so we aren't doing manual mem management.
		free(packFile);

		uniquePointerMap.clear();
	}

	bool overridePhysicsData = false;
	HOOK(void, __fastcall, GenerateRigidBodies, pGenerateRigidBodies, const char** a1, char* a2, unsigned int a3, void* a4)
	{
		if (std::string_view(*a1) != "ghz200_col")
		{
			originalGenerateRigidBodies(a1, a2, a3, a4);
			return;
		}

		if (!s_PhysicsData2022)
			InitPhysData();

		overridePhysicsData = true;
		originalGenerateRigidBodies(a1, a2, a3, a4);
	}

	HOOK(void*, __fastcall, GetHavokClassFromResource, pGetHavokClassFromResource, void* This, const char* name, size_t a3)
	{
		if (!(overridePhysicsData && s_PhysicsData2022 && std::string_view(name) == "hkpPhysicsData"))
			return originalGetHavokClassFromResource(This, name, a3);

		overridePhysicsData = false;
		return s_PhysicsData2022;
	}

    void Init()
    {
		INSTALL_HOOK(GenerateRigidBodies);
		INSTALL_HOOK(GetHavokClassFromResource);

#ifdef _DEBUG_STAGE_VIEW
        // Stage debug view. Credit to EM for finding these offsets & differences from Gens 2011.
         //TODO: Sigscan these offsets.
        uintptr_t lockFlags = 0x14123FB03;
        uintptr_t assertJump = 0x1402CBBB2;
        uintptr_t IsCollisionRender = 0x0141F5241E;

        WRITE_MEMORY(lockFlags, uint32_t, 0x2000)    // Incorrectly set in SXSG
        WRITE_MEMORY(assertJump, uint8_t, 0xEB)      // Assert gets tripped regardless, might be code stripped from 2011. Skip over.
        WRITE_MEMORY(IsCollisionRender, bool, true)
#endif
    }
}
