// For conditions of distribution and use, see copyright notice in License.txt

#include "../IO/FileSystem.h"
#include "../IO/Log.h"
#include "../IO/Stream.h"
#include "../Resource/ResourceCache.h"
#include "ShaderTu.h"
#include "ShaderProgram.h"

#include <algorithm>

ShaderTu::ShaderTu()
{
}

ShaderTu::~ShaderTu()
{
}

void ShaderTu::RegisterObject()
{
    RegisterFactory<ShaderTu>();
}

bool ShaderTu::BeginLoad(Stream& source)
{
    sourceCode.clear();
    return ProcessIncludes(sourceCode, source);
}

bool ShaderTu::EndLoad()
{
    // Release existing variations (if any) to allow them to be recompiled with changed code
    programs.clear();
    return true;
}

void ShaderTu::Define(const std::string& code)
{
    sourceCode = code;
    EndLoad();
}

ShaderProgram* ShaderTu::CreateProgram(const std::string& vsDefinesIn, const std::string& fsDefinesIn)
{
    auto hashPair = std::make_pair(StringHash(vsDefinesIn), StringHash(fsDefinesIn));

    auto it = programs.find(hashPair);
    if (it != programs.end())
        return it->second;
    
    // If initially not found, normalize the defines and try again. Remove unused defines
    std::string vsDefines = NormalizeDefines(vsDefinesIn);
    std::string fsDefines = NormalizeDefines(fsDefinesIn);
    auto normalizedHashPair = std::make_pair(StringHash(vsDefines), StringHash(fsDefines));
    it = programs.find(normalizedHashPair);
    if (it != programs.end())
        return it->second;

    ShaderProgram* newVariation = new ShaderProgram(sourceCode, Name(), vsDefines, fsDefines);
    programs[hashPair] = newVariation;
    programs[normalizedHashPair] = newVariation;
    return newVariation;
}

std::string ShaderTu::NormalizeDefines(const std::string& defines)
{
    std::string ret;
    std::vector<std::string> definesVec = Split(defines, ' ');
    std::sort(definesVec.begin(), definesVec.end());

    for (auto it = definesVec.begin(); it != definesVec.end();)
    {
        bool used = false;

        std::string& str = *it;
        size_t equalsPos = str.find('=');
        if (equalsPos == std::string::npos)
        {
            if (sourceCode.find(str) != std::string::npos)
                used = true;
        }
        else
        {
            std::string beginPart = str.substr(0, equalsPos);
            if (sourceCode.find(beginPart) != std::string::npos)
                used = true;
        }

        if (!used)
            it = definesVec.erase(it);
        else
            ++it;
    }

    for (auto it = definesVec.begin(); it != definesVec.end(); ++it)
    {
        if (it != definesVec.begin())
            ret += " ";
        ret += *it;
    }

    return ret;
}

bool ShaderTu::ProcessIncludes(std::string& code, Stream& source)
{
    ResourceCache* cache = Subsystem<ResourceCache>();

    while (!source.IsEof())
    {
        std::string line = source.ReadLine();

        if (StartsWith(line, "#include"))
        {
            std::string includeFileName = Trim(Path(source.Name()) + Replace(line.substr(9), "\"", ""));
            AutoPtr<Stream> includeStream = cache->OpenResource(includeFileName);
            if (!includeStream)
                return false;

            // Add the include file into the current code recursively
            if (!ProcessIncludes(code, *includeStream))
                return false;
        }
        else
        {
            code += line;
            code += "\n";
        }
    }

    // Finally insert an empty line to mark space between files
    code += "\n";

    return true;
}
