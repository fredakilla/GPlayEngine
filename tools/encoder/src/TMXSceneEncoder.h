#ifndef ENCODER_TMXSCENEEENCODER_H_
#define ENCODER_TMXSCENEEENCODER_H_

#include <fstream>
#include <unordered_map>
#include <tinyxml2/tinyxml2.h>

#include "Base.h"
#include "Vector2.h"
#include "TMXTypes.h"
#include "EncoderArguments.h"
#include "Image.h"

namespace gplayencoder {

/**
 * Class for encoding an TMX file.
 */
class TMXSceneEncoder
{
public:
    /**
     * Constructor.
     */
    TMXSceneEncoder();

    /**
     * Destructor.
     */
    ~TMXSceneEncoder();

    /**
     * Writes out encoded TMX file.
     */
    void write(const EncoderArguments& arguments);

private:
    static std::vector<unsigned int> loadDataElement(const tinyxml2::XMLElement* data);
    static inline std::string buildFilePath(const std::string& directory, const std::string& file);
    static void copyImage(unsigned char* dst, const unsigned char* src,
        unsigned int srcWidth, unsigned int dstWidth, unsigned int bpp,
        unsigned int srcx, unsigned int srcy, unsigned int dstx, unsigned int dsty, unsigned int width, unsigned int height);

    // Parsing
    bool parseTmx(const tinyxml2::XMLDocument& xmlDoc, TMXMap& map, const std::string& inputDirectory) const;
    void parseBaseLayerProperties(const tinyxml2::XMLElement* xmlBaseLayer, TMXBaseLayer* layer) const;

    // Gutter
    void buildTileGutter(TMXMap& map, const std::string& inputDirectory, const std::string& outputDirectory);
    bool buildTileGutterTileset(const TMXTileSet& tileset, const std::string& inputFile, const std::string& outputFile);

    // Writing
    void writeScene(const TMXMap& map, const std::string& outputFilepath, const std::string& sceneName);

    void writeTileset(const TMXMap& map, const TMXLayer* layer, std::ofstream& file);
    void writeSoloTileset(const TMXMap& map, const TMXTileSet& tmxTileset, const TMXLayer& tileset, std::ofstream& file, unsigned int resultOnlyForTileset = TMX_INVALID_ID);

    void writeSprite(const TMXImageLayer* imageLayer, std::ofstream& file);

    void writeNodeProperties(bool enabled, std::ofstream& file, bool seperatorLineWritten = false);
    void writeNodeProperties(bool enabled, const TMXProperties& properties, std::ofstream& file, bool seperatorLineWritten = false);
    void writeNodeProperties(bool enabled, const Vector2& pos, std::ofstream& file, bool seperatorLineWritten = false);
    void writeNodeProperties(bool enabled, const Vector2& pos, const TMXProperties& properties, std::ofstream& file, bool seperatorLineWritten = false);

    void writeLine(std::ofstream& file, const std::string& line) const;

    unsigned int _tabCount;
};

inline void TMXSceneEncoder::writeNodeProperties(bool enabled, std::ofstream& file, bool seperatorLineWritten)
{
    writeNodeProperties(enabled, Vector2::zero(), file, seperatorLineWritten);
}
inline void TMXSceneEncoder::writeNodeProperties(bool enabled, const TMXProperties& properties, std::ofstream& file, bool seperatorLineWritten)
{
    writeNodeProperties(enabled, Vector2::zero(), properties, file, seperatorLineWritten);
}
inline void TMXSceneEncoder::writeNodeProperties(bool enabled, const Vector2& pos, std::ofstream& file, bool seperatorLineWritten)
{
    TMXProperties noOp;
    writeNodeProperties(enabled, pos, noOp, file, seperatorLineWritten);
}

}

#endif
