// For conditions of distribution and use, see copyright notice in License.txt
#include <GL/glew.h>

#include "Graphics.h"
#include "Application.h"

Graphics::Graphics() {
    RegisterSubsystem(this);
    
}

Graphics::~Graphics()
{
    
}
unsigned Graphics::BeginOcclusionQuery(void* object)
{
    GLuint queryId;

    if (freeQueries.size())
    {
        queryId = freeQueries.back();
        freeQueries.pop_back();
    }
    else
        glGenQueries(1, &queryId);

    glBeginQuery(GL_ANY_SAMPLES_PASSED, queryId);
    pendingQueries.push_back(std::make_pair(queryId, object));

    return queryId;
}

void Graphics::EndOcclusionQuery()
{
    glEndQuery(GL_ANY_SAMPLES_PASSED);
}


void Graphics::FreeOcclusionQuery(unsigned queryId)
{
    if (!queryId)
        return;

    for (auto it = pendingQueries.begin(); it != pendingQueries.end(); ++it)
    {
        if (it->first == queryId)
        {
            pendingQueries.erase(it);
            break;
        }
    }

    glDeleteQueries(1, &queryId);
}

void Graphics::CheckOcclusionQueryResults(std::vector<OcclusionQueryResult>& result){
    GLuint available = 0;

    // To save API calls, go through queries in reverse order and assume that if a later query has its result available, then all earlier queries will have too
    for (size_t i = pendingQueries.size() - 1; i < pendingQueries.size(); --i)
    {
        GLuint queryId = pendingQueries[i].first;

        if (!available)
            glGetQueryObjectuiv(queryId, GL_QUERY_RESULT_AVAILABLE, &available);

        if (available)
        {
            GLuint passed = 0;
            glGetQueryObjectuiv(queryId, GL_QUERY_RESULT, &passed);

            OcclusionQueryResult newResult;
            newResult.id = queryId;
            newResult.object = pendingQueries[i].second;
            newResult.visible = passed > 0;
            result.push_back(newResult);

            freeQueries.push_back(queryId);
            pendingQueries.erase(pendingQueries.begin() + i);
        }
    }
}