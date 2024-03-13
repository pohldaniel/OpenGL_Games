#pragma once

#include <vector>
#include <engine/scene/Node.h>


class Octree : public Node
{

};

/*namespace Tree {

	static const size_t NUM_OCTANTS = 8;
	static const unsigned char OF_DRAWABLES_SORT_DIRTY = 0x1;
	static const unsigned char OF_CULLING_BOX_DIRTY = 0x2;


	class WorkQueue;
	struct ReinsertDrawablesTask;

	/// %Octant occlusion query visibility states.
	enum OctantVisibility{
		VIS_OUTSIDE_FRUSTUM = 0,
		VIS_OCCLUDED,
		VIS_OCCLUDED_UNKNOWN,
		VIS_VISIBLE_UNKNOWN,
		VIS_VISIBLE
	};

	/// %Octree cell, contains up to 8 child octants.
	class Octant {
		friend class Octree;

	public:
		/// Construct with defaults.
		Octant();
		/// Destruct. If has a pending occlusion query, free it.
		~Octant();

		/// Initialize parent and bounds.
		void Initialize(Octant* parent, const BoundingBox& boundingBox, unsigned char level, unsigned char childIndex);
		/// Add debug geometry to be rendered.
		void OnRenderDebug(DebugRenderer* debug);
		/// React to occlusion query being rendered for the octant. Store the query ID to know not to re-test until have the result.
		void OnOcclusionQuery(unsigned queryId);
		/// React to occlusion query result. Push changed visibility to parents or children as necessary. If outside frustum, no operation.
		void OnOcclusionQueryResult(bool visible);

		/// Return the culling box. Update as necessary.
		const BoundingBox& CullingBox() const;
		/// Return drawables in this octant.
		const std::vector<Drawable*>& Drawables() const { return drawables; }
		/// Return whether has child octants.
		bool HasChildren() const { return numChildren > 0; }
		/// Return child octant by index.
		Octant* Child(size_t index) const { return children[index]; }
		/// Return parent octant.
		Octant* Parent() const { return parent; }
		/// Return child octant index based on position.
		unsigned char ChildIndex(const Vector3f& position) const { unsigned char ret = position[0] < center[0] ? 0 : 1; ret += position[1] < center[1] ? 0 : 2; ret += position[2] < center[2] ? 0 : 4; return ret; }
		/// Return last occlusion visibility status.
		OctantVisibility Visibility() const { return (OctantVisibility)visibility; }
		/// Return whether is pending an occlusion query result.
		bool OcclusionQueryPending() const { return occlusionQueryId != 0; }
		/// Set bit flag. Called internally.
		void SetFlag(unsigned char bit, bool set) const { if (set) flags |= bit; else flags &= ~bit; }
		/// Test bit flag. Called internally.
		bool TestFlag(unsigned char bit) const { return (flags & bit) != 0; }

		/// Test if a drawable should be inserted in this octant or if a smaller child octant should be created.
		bool FitBoundingBox(const BoundingBox& box, const Vector3f& boxSize) const
		{
			// If max split level, size always OK, otherwise check that box is at least half size of octant
			if (level <= 1 || boxSize.x >= halfSize.x || boxSize.y >= halfSize.y || boxSize.z >= halfSize.z)
				return true;
			// Also check if the box can not fit inside a child octant's culling box, in that case size OK (must insert here)
			else
			{
				Vector3f quarterSize = 0.5f * halfSize;
				if (box.min.x <= fittingBox.min.x + quarterSize[0] || box.max.x >= fittingBox.max.x - quarterSize[0] ||
					box.min.y <= fittingBox.min.y + quarterSize[1] || box.max.y >= fittingBox.max.y - quarterSize[1] ||
					box.max.z <= fittingBox.min.z + quarterSize[2] || box.max.z >= fittingBox.max.z - quarterSize[2])
					return true;
			}

			// Bounding box too small, should create a child octant
			return false;
		}

		/// Mark culling boxes dirty in the parent hierarchy.
		void MarkCullingBoxDirty() const
		{
			const Octant* octant = this;

			while (octant && !octant->TestFlag(OF_CULLING_BOX_DIRTY))
			{
				octant->SetFlag(OF_CULLING_BOX_DIRTY, true);
				octant = octant->parent;
			}
		}

		/// Push visibility status to child octants.
		void PushVisibilityToChildren(Octant* octant, OctantVisibility newVisibility)
		{
			for (size_t i = 0; i < NUM_OCTANTS; ++i)
			{
				if (octant->children[i])
				{
					octant->children[i]->visibility = newVisibility;
					if (octant->children[i]->numChildren)
						PushVisibilityToChildren(octant->children[i], newVisibility);
				}
			}
		}

		/// Set visibility status manually.
		void SetVisibility(OctantVisibility newVisibility, bool pushToChildren = false)
		{
			visibility = newVisibility;

			if (pushToChildren)
				PushVisibilityToChildren(this, newVisibility);
		}

	private:
		/// Combined drawable and child octant bounding box. Used for culling tests.
		mutable BoundingBox cullingBox;
		/// Drawables contained in the octant.
		std::vector<Drawable*> drawables;
		/// Expanded (loose) bounding box used for fitting drawables within the octant.
		BoundingBox fittingBox;
		/// Bounding box center.
		Vector3f center;
		/// Bounding box half size.
		Vector3f halfSize;
		/// Child octants.
		Octant* children[NUM_OCTANTS];
		/// Parent octant.
		Octant* parent;
		/// Last occlusion query visibility.
		OctantVisibility visibility;
		/// Occlusion query id, or 0 if no query pending.
		unsigned occlusionQueryId;
		/// Occlusion query interval timer.
		float occlusionQueryTimer;
		/// Number of child octants.
		unsigned char numChildren;
		/// Subdivision level, decreasing for child octants.
		unsigned char level;
		/// The child index of this octant.
		unsigned char childIndex;
		/// Dirty flags.
		mutable unsigned char flags;
	};
}*/
