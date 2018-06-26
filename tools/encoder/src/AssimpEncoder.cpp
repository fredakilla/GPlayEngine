#include <string>
#include <assimp/Importer.hpp>

#include "AssimpEncoder.h"
#include "Sampler.h"



using namespace gplay;


AssimpEncoder::AssimpEncoder()
{
}

AssimpEncoder::~AssimpEncoder()
{
}

void AssimpEncoder::write(const std::string& filepath, const EncoderArguments& arguments)
{
    _gamePlayFile.adjust();


    std::string outputFilePath = arguments.getOutputFilePath();

    LOG(1, "Saving binary file: %s\n", outputFilePath.c_str());
    _gamePlayFile.saveBinary(outputFilePath);
}


bool AssimpEncoder::writeMaterial(const std::string& filepath)
{

}
