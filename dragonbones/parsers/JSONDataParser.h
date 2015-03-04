#ifndef PARSERS_JSON_DATA_PARSER_H
#define PARSERS_JSON_DATA_PARSER_H

#include "../DragonBones.h"
#include "BaseDataParser.h"

#include "cocostudio/DictionaryHelper.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class JSONDataParser : public BaseDataParser
{
private:
    static bool getBoolean(const rapidjson::Value &root, const char *key, bool defaultValue);
    static float getNumber(const rapidjson::Value &root, const char *key, float defaultValue, float nanValue);
    
private:
    mutable float _textureScale;
    mutable float _armatureScale;
    mutable int _frameRate;
    
public:
    JSONDataParser();
    virtual ~JSONDataParser();
    
    virtual TextureAtlasData* parseTextureAtlasData(const void *rawTextureAtlasData, float scale = 1.f) const;
    
    virtual DragonBonesData* parseDragonBonesData(const void *rawDragonBonesData, float scale = 1.f) const;
    
private:
    TextureData* parseTextureData(const rapidjson::Value &root) const;
    
    ArmatureData* parseArmatureData(const rapidjson::Value &root) const;
    BoneData* parseBoneData(const rapidjson::Value &root) const;
    SkinData* parseSkinData(const rapidjson::Value &root) const;
    SlotData* parseSlotData(const rapidjson::Value &root) const;
    DisplayData* parseDisplayData(const rapidjson::Value &root) const;
    AnimationData* parseAnimationData(const rapidjson::Value &root, const ArmatureData *armatureData) const;
    TransformTimeline* parseTransformTimeline(const rapidjson::Value &root, int duration) const;
    Frame* parseMainFrame(const rapidjson::Value &root) const;
    TransformFrame* parseTransformFrame(const rapidjson::Value &root) const;
    RectangleData* parseRectangleData(const rapidjson::Value &root) const;
    EllipseData* parseEllipseData(const rapidjson::Value &root) const;
    
    void parseTimeline(const rapidjson::Value &root, Timeline &timeline) const;
    void parseFrame(const rapidjson::Value &root, Frame &frame) const;
    void parseTransform(const rapidjson::Value &root, Transform &transform) const;
    void parsePivot(const rapidjson::Value &root, Point &pivot) const;
    void parseColorTransform(const rapidjson::Value &root, ColorTransform &colorTransform) const;
    void parseTextData(const rapidjson::Value &root, TextData &textData) const;
    
private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(JSONDataParser);
};
NAME_SPACE_DRAGON_BONES_END
#endif  // PARSERS_JSON_DATA_PARSER_H
