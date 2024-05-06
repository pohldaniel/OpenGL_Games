#pragma once
#include "../Object/ObjectTu.h"

/// Occlusion query result.
struct OcclusionQueryResult
{
    /// Query ID.
    unsigned id;
    /// Associated object.
    void* object;
    /// Visibility result.
    bool visible;
};

/// %Graphics rendering context and application window.
class Graphics : public ObjectTu
{
    OBJECT(Graphics);

public:
    /// Create window with initial size and register subsystem and object. Rendering context is not created yet.
    Graphics();
    /// Destruct. Closes the application window.
    ~Graphics();


    /// Begin an occlusion query and associate an object with it for checking results. Return the query ID.
    unsigned BeginOcclusionQuery(void* object);
    /// End an occlusion query.
    void EndOcclusionQuery();
    /// Free an occlusion query when its associated object is destroyed early.
    void FreeOcclusionQuery(unsigned id);
    /// Check for and return arrived query results. These are only retained for one frame.
    void CheckOcclusionQueryResults(std::vector<OcclusionQueryResult>& result);
    /// Return number of pending occlusion queries.
    size_t PendingOcclusionQueries() const { return pendingQueries.size(); }

  private:
   
    /// Pending occlusion queries.
    std::vector<std::pair<unsigned, void*> > pendingQueries;
    /// Free occlusion queries.
    std::vector<unsigned> freeQueries;
  
};