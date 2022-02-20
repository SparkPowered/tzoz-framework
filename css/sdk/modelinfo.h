#pragma once

#include "../../util/vmthook.h"
#include "../../util/vec3.h"

using namespace mu;

namespace css
{
	typedef struct model_t;

	typedef enum 
	{
		mod_bad = 0,
		mod_brush,
		mod_sprite,
		mod_studio
	} modtype_t;
	
	// intersection boxes
	struct mstudiobbox_t
	{
		int bone;
		int group;
		vec3 bbmin;
		vec3 bbmax;
		int szhitboxnameindex;
		int unused[8];

		const char *pszHitboxName() const;
	};

	inline const char *mstudiobbox_t::pszHitboxName() const
	{
		return szhitboxnameindex
			? (char*)this + szhitboxnameindex
			: "";
	}

	struct mstudiohitboxset_t
	{
		int sznameindex;
		int numhitboxes;
		int hitboxindex;

		const char *pszName() const;
		mstudiobbox_t *pHitbox (int i) const;
	};

	inline const char *mstudiohitboxset_t::pszName() const
	{
		return (char*)this + sznameindex;
	}

	inline mstudiobbox_t *mstudiohitboxset_t::pHitbox (int i) const
	{
		return i >= 0 && i < numhitboxes
			? (mstudiobbox_t*)((byte*)this + hitboxindex) + i
			: nullptr;
	}

	// bones
	struct mstudiobone_t
	{
		int	sznameindex;		
		int	parent;
		int bonecontroller[6];
		vec3 pos;
		float quat[4];
		vec3 rot;
		vec3 posscale;
		vec3 rotscale;
		matrix3x4_t poseToBone;
		float qAlignment[4];
		int flags;
		int proctype;
		int procindex;
		mutable int physicsbone;
		int surfacepropidx;		
		int contents;
		int unused[8];

		const char *pszName() const;
		const char *pszSurfaceProp() const;
	};

	inline const char *mstudiobone_t::pszName() const
	{
		return (char*)this + sznameindex;
	}

	inline const char *mstudiobone_t::pszSurfaceProp() const
	{
		return (char*)this + surfacepropidx;
	}

	struct studiohdr_t
	{
		// all the specific info about this model is here
		int id;
		int version;
		long checksum;
		char name[64];
		int length;
		vec3 eyeposition;
		vec3 illumposition;
		vec3 hull_min;
		vec3 hull_max;
		vec3 view_bbmin;
		vec3 view_bbmax;
		int flags;
		int numbones;
		int boneindex;
		int numbonecontrollers;
		int bonecontrollerindex;
		int numhitboxsets;
		int hitboxsetindex;
		int numlocalanim;
		int localanimindex;
		int numlocalseq;
		int localseqindex;
		int activitylistversion;
		int eventsindexed;
		int numtextures;
		int textureindex;
		int numcdtextures;
		int cdtextureindex;
		int numskinref;
		int numskinfamilies;
		int skinindex;
		int numbodyparts;
		int bodypartindex;
		int numlocalattachments;
		int localattachmentindex;
		int numlocalnodes;
		int localnodeindex;
		int localnodenameindex;
		int numflexdesc;
		int flexdescindex;
		int numflexcontrollers;
		int flexcontrollerindex;
		int numflexrules;
		int flexruleindex;
		int numikchains;
		int ikchainindex;
		int nummouths;
		int mouthindex;
		int numlocalposeparameters;
		int localposeparamindex;
		int surfacepropindex;
		int keyvalueindex;
		int keyvaluesize;
		int numlocalikautoplaylocks;
		int localikautoplaylockindex;
		float mass;
		int contents;
		int numincludemodels;
		int includemodelindex;
		void *virtualModel;
		int szanimblocknameindex;
		int numanimblocks;
		int animblockindex;
		void *animblockModel;
		int bonetablebynameindex;
		void *pVertexBase;
		void *pIndexBase;
		byte constdirectionallightdot;
		byte rootLOD;
		byte unused[2];
		int zeroframecacheindex;
		int unused2[6];

		mstudiobone_t *pBone (int i) const;
		mstudiohitboxset_t *pHitboxSet (int i) const;
	};

	inline mstudiobone_t *studiohdr_t::pBone (int i) const
	{
		return i >= 0 && i < numbones
			? (mstudiobone_t*)((byte*)this + boneindex) + i
			: nullptr;
	}

	inline mstudiohitboxset_t *studiohdr_t::pHitboxSet (int i) const
	{
		return i >= 0 && i < numhitboxsets
			? (mstudiohitboxset_t*)((byte*)this + hitboxsetindex) + i
			: nullptr;
	}
	
	class CMaterial : public vmtobject {
	public:
		CMaterial() : vmtobject() {}

		void SetMaterialVarFlag (int flag, bool on);
	};

	inline void CMaterial::SetMaterialVarFlag (int flag, bool on)
	{
		method<void (__thiscall*)(void*, int, bool)>(29)(m_ptr, flag, on);
	}

	class CModelInfo : public vmthook {
	public:
		CModelInfo() : vmthook() {}

		string GetModelName (const model_t *model);
		int GetModelType (const model_t *model);
		void GetModelMaterials (const model_t *model, int numtextures, void **mats);
		studiohdr_t *GetStudioModel (const model_t *model);
		
		mstudiobbox_t *GetStudioBBox (const model_t *model, int set, int hitbox);
	};

	inline string CModelInfo::GetModelName (const model_t *model)
	{
		return old_method<char *(__thiscall*)(void*, const model_t*)>(3)(inst(), model);
	}

	inline int CModelInfo::GetModelType (const model_t *model)
	{
		return old_method<int (__thiscall*)(void*, const model_t*)>(9)(inst(), model);
	}

	inline void CModelInfo::GetModelMaterials (const model_t *model, int numtextures, void **mats)
	{
		old_method<void (__thiscall*)(void*, const model_t*, int, void**)>(16)(inst(), model, numtextures, mats);
	}

	inline studiohdr_t *CModelInfo::GetStudioModel (const model_t *model)
	{
		return old_method<studiohdr_t *(__thiscall*)(void*, const model_t*)>(28)(inst(), model);
	}
	
	inline mstudiobbox_t *CModelInfo::GetStudioBBox (const model_t *model, int set, int hitbox)
	{
		studiohdr_t *hdr = GetStudioModel(model);
		
		if (hdr != nullptr)
		{
			mstudiohitboxset_t *boxset = hdr->pHitboxSet(set);
			
			if (boxset != nullptr)
				return boxset->pHitbox(hitbox);
		}
		
		return nullptr;
	}
};