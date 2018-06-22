#include "../core/Base.h"
#include "../graphics/Effect.h"
#include "../core/FileSystem.h"
#include "../core/Game.h"

#define OPENGL_ES_DEFINE  "OPENGL_ES"

#define INVALID_VS "res/core/shaders/invalid.vert"
#define INVALID_FS "res/core/shaders/invalid.frag"

namespace gplay {

// Cache of unique effects.
static std::map<std::string, Effect*> __effectCache;
static Effect* __currentEffect = nullptr;

// Invalid effect used by default to replace shaders in error.
Effect* Effect::_invalidEffect = nullptr;


Effect::Effect() : _gpuProgram(0)
{
}

Effect::~Effect()
{
    // Remove this effect from the cache.
    __effectCache.erase(_id);

    // Free uniforms.
    for (std::map<std::string, Uniform*>::iterator itr = _uniforms.begin(); itr != _uniforms.end(); ++itr)
    {
        SAFE_DELETE(itr->second);
    }

    if (_gpuProgram)
    {
        // If our program object is currently bound, unbind it before we're destroyed.
        if (__currentEffect == this)
            __currentEffect = nullptr;
        SAFE_DELETE(_gpuProgram);
    }
}

Effect* Effect::createFromFile(const char* vshPath, const char* fshPath, const char* defines, bool useCache)
{
    GP_ASSERT(vshPath);
    GP_ASSERT(fshPath);

    // Search the effect cache for an identical effect that is already loaded.
    std::string uniqueId = vshPath;
    uniqueId += ';';
    uniqueId += fshPath;
    uniqueId += ';';
    if (defines)
    {
        uniqueId += defines;
    }

    if(useCache)
    {
        std::map<std::string, Effect*>::const_iterator itr = __effectCache.find(uniqueId);
        if (itr != __effectCache.end())
        {
            // Found an exiting effect with this id, so increase its ref count and return it.
            GP_ASSERT(itr->second);
            itr->second->addRef();
            return itr->second;
        }
    }


    // Create gpu program.
    BGFXGpuProgram * _gpuProgram = new BGFXGpuProgram();
    bool success = _gpuProgram->set(vshPath, fshPath, defines);

    if(!success)
    {
        return _invalidEffect;
    }

    // Create and return the new Effect.
    Effect* effect = new Effect();
    effect->_gpuProgram = _gpuProgram;

    // Query and store uniforms from the program.
    unsigned int activeUniforms = _gpuProgram->getUniformsInfo().size();
    if (activeUniforms > 0)
    {
        unsigned int samplerIndex = 0;
        for (int i = 0; i < activeUniforms; ++i)
        {
            Uniform::UniformInfo info = _gpuProgram->getUniformsInfo()[i];

            BGFXUniform* uniform = new BGFXUniform(info.name.c_str(), info.type, info.num);
            uniform->_effect = effect;
            uniform->_name = info.name;
            //uniform->_location = info.uniformLocation;
            uniform->_type = info.type;
            if (info.type == Uniform::UniformType::UT_SAMPLER)
            {
                uniform->_index = samplerIndex++;
                //samplerIndex += info.uniformSize;
            }
            else
            {
                uniform->_index = 0;
            }

            effect->_uniforms[info.name] = uniform;
        }
    }


    // Store this effect in the cache.
    effect->_id = uniqueId;
    __effectCache[uniqueId] = effect;

    return effect;
}

static void replaceDefines(const char* defines, std::string& out)
{
    Properties* graphicsConfig = Game::getInstance()->getConfig()->getNamespace("graphics", true);
    const char* globalDefines = graphicsConfig ? graphicsConfig->getString("shaderDefines") : NULL;

    // Build full semicolon delimited list of defines
#ifdef OPENGL_ES
    out = OPENGL_ES_DEFINE;
#else
    out = "";
#endif
    if (globalDefines && strlen(globalDefines) > 0)
    {
        if (out.length() > 0)
            out += ';';
        out += globalDefines;
    }
    if (defines && strlen(defines) > 0)
    {
        if (out.length() > 0)
            out += ';';
        out += defines;
    }

    // Replace semicolons
    if (out.length() > 0)
    {
        size_t pos;
        out.insert(0, "#define ");
        while ((pos = out.find(';')) != std::string::npos)
        {
            out.replace(pos, 1, "\n#define ");
        }
        out += "\n";
    }
}

static void replaceIncludes(const char* filepath, const char* source, std::string& out)
{
    // Replace the #include "xxxx.xxx" with the sourced file contents of "filepath/xxxx.xxx"
    std::string str = source;
    size_t lastPos = 0;
    size_t headPos = 0;
    size_t fileLen = str.length();
    size_t tailPos = fileLen;
    while (headPos < fileLen)
    {
        lastPos = headPos;
        if (headPos == 0)
        {
            // find the first "#include"
            headPos = str.find("#include");
        }
        else
        {
            // find the next "#include"
            headPos = str.find("#include", headPos + 1);
        }

        // If "#include" is found
        if (headPos != std::string::npos)
        {
            // append from our last position for the legth (head - last position) 
            out.append(str.substr(lastPos,  headPos - lastPos));

            // find the start quote "
            size_t startQuote = str.find("\"", headPos) + 1;
            if (startQuote == std::string::npos)
            {
                // We have started an "#include" but missing the leading quote "
                GP_ERROR("Compile failed for shader '%s' missing leading \".", filepath);
                return;
            }
            // find the end quote "
            size_t endQuote = str.find("\"", startQuote);
            if (endQuote == std::string::npos)
            {
                // We have a start quote but missing the trailing quote "
                GP_ERROR("Compile failed for shader '%s' missing trailing \".", filepath);
                return;
            }

            // jump the head position past the end quote
            headPos = endQuote + 1;
            
            // File path to include and 'stitch' in the value in the quotes to the file path and source it.
            std::string filepathStr = filepath;
            std::string directoryPath = filepathStr.substr(0, filepathStr.rfind('/') + 1);
            size_t len = endQuote - (startQuote);
            std::string includeStr = str.substr(startQuote, len);
            directoryPath.append(includeStr);
            const char* includedSource = FileSystem::readAll(directoryPath.c_str());
            if (includedSource == NULL)
            {
                GP_ERROR("Compile failed for shader '%s' invalid filepath.", filepathStr.c_str());
                return;
            }
            else
            {
                // Valid file so lets attempt to see if we need to append anything to it too (recurse...)
                replaceIncludes(directoryPath.c_str(), includedSource, out);
                SAFE_DELETE_ARRAY(includedSource);
            }
        }
        else
        {
            // Append the remaining
            out.append(str.c_str(), lastPos, tailPos);
        }
    }
}

static void writeShaderToErrorFile(const char* filePath, const char* source)
{
    std::string path = filePath;
    path += ".err";
    std::unique_ptr<Stream> stream(FileSystem::open(path.c_str(), FileSystem::WRITE));
    if (stream.get() != NULL && stream->canWrite())
    {
        stream->write(source, 1, strlen(source));
    }
}

const char* Effect::getId() const
{
    return _id.c_str();
}

Uniform* Effect::getUniform(const char* name) const
{
    std::map<std::string, Uniform*>::const_iterator itr = _uniforms.find(name);

    if (itr != _uniforms.end())
    {
		// Return cached uniform variable
		return itr->second;
    }

	// No uniform variable found - return NULL
	return NULL;
}

Uniform* Effect::getUniform(unsigned int index) const
{
    unsigned int i = 0;
    for (std::map<std::string, Uniform*>::const_iterator itr = _uniforms.begin(); itr != _uniforms.end(); ++itr, ++i)
    {
        if (i == index)
        {
            return itr->second;
        }
    }
    return NULL;
}

unsigned int Effect::getUniformCount() const
{
    return (unsigned int)_uniforms.size();
}

const BGFXGpuProgram *Effect::getGpuProgram() const
{
    return _gpuProgram;
}

BGFXGpuProgram *Effect::getGpuProgram()
{
    return _gpuProgram;
}

void Effect::bind()
{
    __currentEffect = this;
}

Effect* Effect::getCurrentEffect()
{
    return __currentEffect;
}

void Effect::initialize()
{
    // create invalid shader
    Effect::_invalidEffect = Effect::createFromFile(INVALID_VS, INVALID_FS);
    GP_ASSERT(Effect::_invalidEffect);
}

Effect* Effect::GetInvalidEffect()
{
    return _invalidEffect;
}

}
