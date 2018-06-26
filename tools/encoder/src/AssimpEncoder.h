#ifndef ASSIMPCODER_H_
#define ASSIMPCODER_H_

#include "GPBFile.h"
#include "EncoderArguments.h"

using namespace gplay;

/**
 * Class for binary encoding 3d files supported by assimp library
 */
class AssimpEncoder
{
public:

    /**
     * Constructor.
     */
    AssimpEncoder();

    /**
     * Destructor.
     */
    ~AssimpEncoder();

    /**
     * Writes out encoded FBX file.
     */
    void write(const std::string& filepath, const EncoderArguments& arguments);

    /**
     * Writes a material file.
     *
     * @param filepath
     *
     * @return True if successful; false otherwise.
     */
    bool writeMaterial(const std::string& filepath);

private:

    /**
     * The GPlay file that is populated while reading the FBX file.
     */
    GPBFile _gamePlayFile;
};

#endif // ASSIMPCODER_H_
