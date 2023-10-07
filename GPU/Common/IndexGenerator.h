// Copyright (c) 2012- PPSSPP Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0 or later versions.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official git repository and contact information can be found at
// https://github.com/hrydgard/ppsspp and http://www.ppsspp.org/.


#pragma once

#include "Common/CommonTypes.h"
#include "Common/Swap.h"
#include "GPU/ge_constants.h"

class IndexGenerator {
public:
	void Setup(u16 *indexptr);
	void Reset() {
		prim_ = GE_PRIM_INVALID;
		seenPrims_ = 0;
		pureCount_ = 0;
		this->inds_ = indsBase_;
	}

	bool PrimCompatible(int prim1, int prim2) {
		if (prim1 == GE_PRIM_INVALID || prim2 == GE_PRIM_KEEP_PREVIOUS)
			return true;
		return indexedPrimitiveType[prim1] == indexedPrimitiveType[prim2];
	}

	bool PrimCompatible(int prim) const {
		if (prim_ == GE_PRIM_INVALID || prim == GE_PRIM_KEEP_PREVIOUS)
			return true;
		return indexedPrimitiveType[prim] == prim_;
	}

	GEPrimitiveType Prim() const { return prim_; }
	GEPrimitiveType GeneralPrim() const {
		switch (prim_) {
		case GE_PRIM_LINE_STRIP: return GE_PRIM_LINES; break;
		case GE_PRIM_TRIANGLE_STRIP:
		case GE_PRIM_TRIANGLE_FAN: return GE_PRIM_TRIANGLES; break;
		default:
			return prim_;
		}
	}

	void AddPrim(int prim, int vertexCount, int indexOffset, bool clockwise);
	void TranslatePrim(int prim, int numInds, const u8 *inds, int indexOffset, bool clockwise);
	void TranslatePrim(int prim, int numInds, const u16_le *inds, int indexOffset, bool clockwise);
	void TranslatePrim(int prim, int numInds, const u32_le *inds, int indexOffset, bool clockwise);

	int VertexCount() const { return inds_ - indsBase_; }
	int SeenPrims() const { return seenPrims_; }
	int PureCount() const { return pureCount_; }
	bool SeenOnlyPurePrims() const {
		return seenPrims_ == (1 << GE_PRIM_TRIANGLES) ||
			seenPrims_ == (1 << GE_PRIM_LINES) ||
			seenPrims_ == (1 << GE_PRIM_POINTS) ||
			seenPrims_ == (1 << GE_PRIM_TRIANGLE_STRIP);
	}

private:
	// Points (why index these? code simplicity)
	void AddPoints(int numVerts, int indexOffset);
	// Triangles
	void AddList(int numVerts, int indexOffset, bool clockwise);
	void AddStrip(int numVerts, int indexOffset, bool clockwise);
	void AddFan(int numVerts, int indexOffset, bool clockwise);
	// Lines
	void AddLineList(int numVerts, int indexOffset);
	void AddLineStrip(int numVerts, int indexOffset);
	// Rectangles
	void AddRectangles(int numVerts, int indexOffset);

	// These translate already indexed lists
	template <class ITypeLE, int flag>
	void TranslatePoints(int numVerts, const ITypeLE *inds, int indexOffset);
	template <class ITypeLE, int flag>
	void TranslateList(int numVerts, const ITypeLE *inds, int indexOffset, bool clockwise);
	template <class ITypeLE, int flag>
	inline void TranslateLineList(int numVerts, const ITypeLE *inds, int indexOffset);
	template <class ITypeLE, int flag>
	inline void TranslateLineStrip(int numVerts, const ITypeLE *inds, int indexOffset);

	template <class ITypeLE, int flag>
	void TranslateStrip(int numVerts, const ITypeLE *inds, int indexOffset, bool clockwise);
	template <class ITypeLE, int flag>
	void TranslateFan(int numVerts, const ITypeLE *inds, int indexOffset, bool clockwise);

	template <class ITypeLE, int flag>
	inline void TranslateRectangles(int numVerts, const ITypeLE *inds, int indexOffset);

	enum {
		SEEN_INDEX8 = 1 << 16,
		SEEN_INDEX16 = 1 << 17,
		SEEN_INDEX32 = 1 << 18,
	};

	u16 *indsBase_;
	u16 *inds_;
	int pureCount_;
	GEPrimitiveType prim_;
	int seenPrims_;

	static const u8 indexedPrimitiveType[7];
};

