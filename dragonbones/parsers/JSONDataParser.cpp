#include "JSONDataParser.h"
#include "ConstValues.h"

NAME_SPACE_DRAGON_BONES_BEGIN

#define PARSER cocostudio::DictionaryHelper::getInstance()

bool JSONDataParser::getBoolean(const rapidjson::Value &root, const char *key, bool defaultValue)
{
    if ( PARSER->checkObjectExist_json(root, key) )
    {
//        const char *value = data.Attribute(key);
        
        if (
//            strcmp(value, "0") == 0 ||
//            strcmp(value, "NaN") == 0 ||
//            strcmp(value, "") == 0 ||
//            strcmp(value, "false") == 0 ||
//            strcmp(value, "null") == 0 ||
//            strcmp(value, "undefined") == 0
            root[key].IsString()
            )
        {
            return false;
        }
        else
        {
            return PARSER->getIntValue_json(root, key);
        }
    }
    
    return defaultValue;
}

float JSONDataParser::getNumber(const rapidjson::Value &root, const char *key, float defaultValue, float nanValue)
{
    if ( PARSER->checkObjectExist_json(root, key) )
    {
//        const char *value = data.Attribute(key);
        
        if (
//            strcmp(value, "NaN") == 0 ||
//            strcmp(value, "") == 0 ||
//            strcmp(value, "false") == 0 ||
//            strcmp(value, "null") == 0 ||
//            strcmp(value, "undefined") == 0
            root[key].IsString()
            )
        {
            return nanValue;
        }
        else
        {
            return PARSER->getFloatValue_json(root, key);
        }
    }
    
    return defaultValue;
}

JSONDataParser::JSONDataParser()
{
    _textureScale = 1.f;
    _armatureScale = 1.f;
    _frameRate = 30;
}
JSONDataParser::~JSONDataParser() {}

TextureAtlasData* JSONDataParser::parseTextureAtlasData(const void *rawTextureAtlasData, float scale) const
{
    _textureScale = scale;
    const rapidjson::Value& root = (rapidjson::Value&)*rawTextureAtlasData;
    TextureAtlasData *textureAtlasData = new TextureAtlasData();
    const char *name = PARSER->getStringValue_json(root, ConstValues::A_NAME.c_str());
    textureAtlasData->name = name ? name : "";
    textureAtlasData->imagePath = PARSER->getStringValue_json(root, ConstValues::A_IMAGE_PATH.c_str());
    
    int length = PARSER->getArrayCount_json(root, ConstValues::SUB_TEXTURE.c_str());
    for(int i=0; i<length; i++)
    {
        TextureData *textureData = parseTextureData(PARSER->getSubDictionary_json(root, ConstValues::SUB_TEXTURE.c_str(), i));
        textureAtlasData->textureDataList.push_back(textureData);
    }
    
    return textureAtlasData;
}

TextureData* JSONDataParser::parseTextureData(const rapidjson::Value &root) const
{
    TextureData *textureData = new TextureData();
    textureData->name = PARSER->getStringValue_json(root, ConstValues::A_NAME.c_str());
    textureData->rotated = PARSER->getBooleanValue_json(root, ConstValues::A_ROTATED.c_str());
    textureData->region.x = PARSER->getFloatValue_json(root, ConstValues::A_X.c_str()) / _textureScale;
    textureData->region.y = PARSER->getFloatValue_json(root, ConstValues::A_Y.c_str()) / _textureScale;
    textureData->region.width = PARSER->getFloatValue_json(root, ConstValues::A_WIDTH.c_str()) / _textureScale;
    textureData->region.height = PARSER->getFloatValue_json(root, ConstValues::A_HEIGHT.c_str()) / _textureScale;
    const float frameWidth = PARSER->getFloatValue_json(root, ConstValues::A_FRAME_WIDTH.c_str()) / _textureScale;
    const float frameHeight = PARSER->getFloatValue_json(root, ConstValues::A_FRAME_HEIGHT.c_str()) / _textureScale;
    
    if (frameWidth > 0 && frameHeight > 0)
    {
        textureData->frame = new Rectangle();
        textureData->frame->x = PARSER->getFloatValue_json(root, ConstValues::A_FRAME_X.c_str()) / _textureScale;
        textureData->frame->y = PARSER->getFloatValue_json(root, ConstValues::A_FRAME_Y.c_str()) / _textureScale;
        textureData->frame->width = frameWidth;
        textureData->frame->height = frameHeight;
    }
    
    return textureData;
}

DragonBonesData* JSONDataParser::parseDragonBonesData(const void *rawDragonBonesData, float scale) const
{
    _armatureScale = scale;
    const rapidjson::Value& root = (rapidjson::Value&)*rawDragonBonesData;
//    std::string version = PARSER->getStringValue_json(root, ConstValues::A_VERSION.c_str());
    // TODO
    /*
     switch(version)
     {
     
     }
     */
    _frameRate = PARSER->getIntValue_json(root, ConstValues::A_FRAME_RATE.c_str());
    DragonBonesData *dragonBonesData = new DragonBonesData();
    dragonBonesData->name = PARSER->getStringValue_json(root, ConstValues::A_NAME.c_str());
    
    int length = PARSER->getArrayCount_json(root, ConstValues::ARMATURE.c_str());
    for(int i=0; i<length; i++)
    {
        ArmatureData *armatureData = parseArmatureData(PARSER->getSubDictionary_json(root, ConstValues::ARMATURE.c_str(), i));
        dragonBonesData->armatureDataList.push_back(armatureData);
    }
    
    return dragonBonesData;
}

ArmatureData* JSONDataParser::parseArmatureData(const rapidjson::Value &root) const
{
    ArmatureData *armatureData = new ArmatureData();
    armatureData->name = PARSER->getStringValue_json(root, ConstValues::A_NAME.c_str());
    
    int length = PARSER->getArrayCount_json(root, ConstValues::BONE.c_str());
    for(int i=0; i<length; i++)
    {
        BoneData *boneData = parseBoneData(PARSER->getSubDictionary_json(root, ConstValues::BONE.c_str(), i));
        armatureData->boneDataList.push_back(boneData);
    }
    
    length = PARSER->getArrayCount_json(root, ConstValues::SKIN.c_str());
    for(int i=0; i<length; i++)
    {
        SkinData *skinData = parseSkinData(PARSER->getSubDictionary_json(root, ConstValues::SKIN.c_str(), i));
        armatureData->skinDataList.push_back(skinData);
    }
    
    transformArmatureData(armatureData);
    armatureData->sortBoneDataList();
    
    length = PARSER->getArrayCount_json(root, ConstValues::ANIMATION.c_str());
    for(int i=0; i<length; i++)
    {
        AnimationData *animationData = parseAnimationData(PARSER->getSubDictionary_json(root, ConstValues::ANIMATION.c_str(), i), armatureData);
        armatureData->animationDataList.push_back(animationData);
    }
    
    length = PARSER->getArrayCount_json(root, ConstValues::RECTANGLE.c_str());
    for(int i=0; i<length; i++)
    {
        RectangleData *rectangleData = parseRectangleData(PARSER->getSubDictionary_json(root, ConstValues::RECTANGLE.c_str(), i));
        armatureData->areaDataList.push_back(rectangleData);
    }
    
    length = PARSER->getArrayCount_json(root, ConstValues::ELLIPSE.c_str());
    for(int i=0; i<length; i++)
    {
        EllipseData *ellipseData = parseEllipseData(PARSER->getSubDictionary_json(root, ConstValues::ELLIPSE.c_str(), i));
        armatureData->areaDataList.push_back(ellipseData);
    }
    
    return armatureData;
}

BoneData* JSONDataParser::parseBoneData(const rapidjson::Value &root) const
{
    BoneData *boneData = new BoneData();
    boneData->name = PARSER->getStringValue_json(root, ConstValues::A_NAME.c_str());
    const char *parent = PARSER->getStringValue_json(root, ConstValues::A_PARENT.c_str());
    
    if (parent)
    {
        boneData->parent = parent;
    }
    
    boneData->length = PARSER->getFloatValue_json(root, ConstValues::A_LENGTH.c_str());
    boneData->inheritRotation = getBoolean(root, ConstValues::A_INHERIT_ROTATION.c_str(), true);
    boneData->inheritScale = getBoolean(root, ConstValues::A_INHERIT_SCALE.c_str(), false);
    
    if ( PARSER->checkObjectExist_json(root, ConstValues::TRANSFORM.c_str()) )
    {
        parseTransform(PARSER->getSubDictionary_json(root, ConstValues::TRANSFORM.c_str()), boneData->global);
    }
    
    boneData->transform = boneData->global;
    
    int length = PARSER->getArrayCount_json(root, ConstValues::RECTANGLE.c_str());
    for(int i=0; i<length; i++)
    {
        RectangleData *rectangleData = parseRectangleData(PARSER->getSubDictionary_json(root, ConstValues::RECTANGLE.c_str(), i));
        boneData->areaDataList.push_back(rectangleData);
    }
    
    length = PARSER->getArrayCount_json(root, ConstValues::ELLIPSE.c_str());
    for(int i=0; i<length; i++)
    {
        EllipseData *ellipseData = parseEllipseData(PARSER->getSubDictionary_json(root, ConstValues::ELLIPSE.c_str(), i));
        boneData->areaDataList.push_back(ellipseData);
    }
    
    return boneData;
}

SkinData* JSONDataParser::parseSkinData(const rapidjson::Value &root) const
{
    SkinData *skinData = new SkinData();
    skinData->name = PARSER->getStringValue_json(root, ConstValues::A_NAME.c_str());
    
    int length = PARSER->getArrayCount_json(root, ConstValues::SLOT.c_str());
    for(int i=0; i<length; i++)
    {
        SlotData *slotData = parseSlotData(PARSER->getSubDictionary_json(root, ConstValues::SLOT.c_str(), i));
        skinData->slotDataList.push_back(slotData);
    }
    
    return skinData;
}

SlotData* JSONDataParser::parseSlotData(const rapidjson::Value &root) const
{
    SlotData *slotData = new SlotData();
    slotData->name = PARSER->getStringValue_json(root, ConstValues::A_NAME.c_str());
    slotData->parent = PARSER->getStringValue_json(root, ConstValues::A_PARENT.c_str());
    slotData->zOrder = PARSER->getFloatValue_json(root, ConstValues::A_Z_ORDER.c_str());
    
    if ( PARSER->checkObjectExist_json(root, ConstValues::A_BLENDMODE.c_str()) )
    {
        slotData->blendMode = getBlendModeByString(PARSER->getStringValue_json(root, ConstValues::A_BLENDMODE.c_str()));
    }
    
    int length = PARSER->getArrayCount_json(root, ConstValues::DISPLAY.c_str());
    for(int i=0; i<length; i++)
    {
        DisplayData *displayData = parseDisplayData(PARSER->getSubDictionary_json(root, ConstValues::DISPLAY.c_str(), i));
        slotData->displayDataList.push_back(displayData);
    }
    
    return slotData;
}

DisplayData* JSONDataParser::parseDisplayData(const rapidjson::Value &root) const
{
    DisplayData *displayData = new DisplayData();
    displayData->name = PARSER->getStringValue_json(root, ConstValues::A_NAME.c_str());
    displayData->type = getDisplayTypeByString(PARSER->getStringValue_json(root, ConstValues::A_TYPE.c_str()));
    
    if ( PARSER->checkObjectExist_json(root, ConstValues::SCALING_GRID.c_str()) )
    {
        const rapidjson::Value &color = PARSER->getSubDictionary_json(root, ConstValues::SCALING_GRID.c_str());
        displayData->scalingGrid = true;
        displayData->scalingGridLeft = PARSER->getIntValue_json(color, ConstValues::A_LEFT.c_str());
        displayData->scalingGridRight = PARSER->getIntValue_json(color, ConstValues::A_RIGHT.c_str());
        displayData->scalingGridTop = PARSER->getIntValue_json(color, ConstValues::A_TOP.c_str());
        displayData->scalingGridBottom = PARSER->getIntValue_json(color, ConstValues::A_BOTTOM.c_str());
    }
    else
    {
        displayData->scalingGrid = false;
    }
    
    if ( PARSER->checkObjectExist_json(root, ConstValues::TRANSFORM.c_str()) )
    {
        parseTransform(PARSER->getSubDictionary_json(root, ConstValues::TRANSFORM.c_str()), displayData->transform);
        parsePivot(PARSER->getSubDictionary_json(root, ConstValues::TRANSFORM.c_str()), displayData->pivot);
    }
    
    if ( PARSER->checkObjectExist_json(root, ConstValues::TEXT.c_str()) )
    {
        displayData->textData = new TextData();
        parseTextData(PARSER->getSubDictionary_json(root, ConstValues::TEXT.c_str()), *displayData->textData);
    }
    
    return displayData;
}

AnimationData* JSONDataParser::parseAnimationData(const rapidjson::Value &root, const ArmatureData *armatureData) const
{
    AnimationData *animationData = new AnimationData();
    animationData->name = PARSER->getStringValue_json(root, ConstValues::A_NAME.c_str());
    animationData->frameRate = _frameRate;
    animationData->duration = (int)(round(PARSER->getIntValue_json(root, ConstValues::A_DURATION.c_str()) * 1000.f / _frameRate));
    animationData->playTimes = PARSER->getIntValue_json(root, ConstValues::A_LOOP.c_str());
    animationData->fadeTime = PARSER->getFloatValue_json(root, ConstValues::A_FADE_IN_TIME.c_str());
    animationData->scale = getNumber(root, ConstValues::A_SCALE.c_str(), 1.f, 1.f);
    // use frame tweenEase, NaN
    // overwrite frame tweenEase, [-1, 0):ease in, 0:line easing, (0, 1]:ease out, (1, 2]:ease in out
    animationData->tweenEasing = getNumber(root, ConstValues::A_TWEEN_EASING.c_str(), USE_FRAME_TWEEN_EASING, USE_FRAME_TWEEN_EASING);
    animationData->autoTween = getBoolean(root, ConstValues::A_AUTO_TWEEN.c_str(), true);
    
    int length = PARSER->getArrayCount_json(root, ConstValues::FRAME.c_str());
    for(int i=0; i<length; i++)
    {
        Frame *frame = parseMainFrame(PARSER->getSubDictionary_json(root, ConstValues::FRAME.c_str(), i));
        animationData->frameList.push_back(frame);
    }
    
    parseTimeline(root, *animationData);
    
    length = PARSER->getArrayCount_json(root, ConstValues::TIMELINE.c_str());
    for(int i=0; i<length; i++)
    {
        TransformTimeline *timeline = parseTransformTimeline(PARSER->getSubDictionary_json(root, ConstValues::TIMELINE.c_str(), i), animationData->duration);
        animationData->timelineList.push_back(timeline);
    }
    
    addHideTimeline(animationData, armatureData);
    transformAnimationData(animationData, armatureData);
    return animationData;
}

TransformTimeline* JSONDataParser::parseTransformTimeline(const rapidjson::Value &root, int duration) const
{
    TransformTimeline *timeline = new TransformTimeline();
    timeline->name = PARSER->getStringValue_json(root, ConstValues::A_NAME.c_str());
    timeline->scale = PARSER->getFloatValue_json(root, ConstValues::A_SCALE.c_str());
    timeline->offset = PARSER->getFloatValue_json(root, ConstValues::A_OFFSET.c_str());
    timeline->duration = duration;
    
    int length = PARSER->getArrayCount_json(root, ConstValues::FRAME.c_str());
    for(int i=0; i<length; i++)
    {
        TransformFrame *frame = parseTransformFrame(PARSER->getSubDictionary_json(root, ConstValues::FRAME.c_str(), i));
        timeline->frameList.push_back(frame);
    }
    
    parseTimeline(root, *timeline);
    return timeline;
}

Frame* JSONDataParser::parseMainFrame(const rapidjson::Value &root) const
{
    Frame *frame = new Frame();
    parseFrame(root, *frame);
    return frame;
}

TransformFrame* JSONDataParser::parseTransformFrame(const rapidjson::Value &root) const
{
    TransformFrame *frame = new TransformFrame();
    parseFrame(root, *frame);
    
    frame->visible = !getBoolean(root, ConstValues::A_HIDE.c_str(), false);
    // NaN:no tween, 10:auto tween, [-1, 0):ease in, 0:line easing, (0, 1]:ease out, (1, 2]:ease in out
    frame->tweenEasing = getNumber(root, ConstValues::A_TWEEN_EASING.c_str(), AUTO_TWEEN_EASING, NO_TWEEN_EASING);
    frame->tweenRotate = PARSER->getIntValue_json(root, ConstValues::A_TWEEN_ROTATE.c_str());
    frame->tweenScale = getBoolean(root, ConstValues::A_TWEEN_SCALE.c_str(), true);
    frame->displayIndex = PARSER->getIntValue_json(root, ConstValues::A_DISPLAY_INDEX.c_str());
    frame->zOrder = getNumber(root, ConstValues::A_Z_ORDER.c_str(), 0.f, 0.f);
    
    if ( PARSER->checkObjectExist_json(root, ConstValues::TRANSFORM.c_str()) )
    {
        parseTransform(PARSER->getSubDictionary_json(root, ConstValues::TRANSFORM.c_str()), frame->global);
        parsePivot(PARSER->getSubDictionary_json(root, ConstValues::TRANSFORM.c_str()), frame->pivot);
    }
    
    // copy
    frame->transform = frame->global;
    frame->scaleOffset.x = getNumber(root, ConstValues::A_SCALE_X_OFFSET.c_str(), 0.f, 0.f);
    frame->scaleOffset.y = getNumber(root, ConstValues::A_SCALE_Y_OFFSET.c_str(), 0.f, 0.f);
    
    if ( PARSER->checkObjectExist_json(root, ConstValues::COLOR_TRANSFORM.c_str()) )
    {
        frame->color = new ColorTransform();
        parseColorTransform(PARSER->getSubDictionary_json(root, ConstValues::COLOR_TRANSFORM.c_str()), *frame->color);
    }
    
    return frame;
}

RectangleData* JSONDataParser::parseRectangleData(const rapidjson::Value &root) const
{
    RectangleData *rectangleData = new RectangleData();
    rectangleData->name = PARSER->getStringValue_json(root, ConstValues::A_NAME.c_str());
    rectangleData->width = PARSER->getFloatValue_json(root, ConstValues::A_WIDTH.c_str());
    rectangleData->height = PARSER->getFloatValue_json(root, ConstValues::A_HEIGHT.c_str());
    
    if ( PARSER->checkObjectExist_json(root, ConstValues::TRANSFORM.c_str()) )
    {
        parseTransform(PARSER->getSubDictionary_json(root, ConstValues::TRANSFORM.c_str()), rectangleData->transform);
        parsePivot(PARSER->getSubDictionary_json(root, ConstValues::TRANSFORM.c_str()), rectangleData->pivot);
    }
    
    return rectangleData;
}

EllipseData* JSONDataParser:: parseEllipseData(const rapidjson::Value &root) const
{
    EllipseData *ellipseData = new EllipseData();
    ellipseData->name = PARSER->getStringValue_json(root, ConstValues::A_NAME.c_str());
    ellipseData->width = PARSER->getFloatValue_json(root, ConstValues::A_WIDTH.c_str());
    ellipseData->height = PARSER->getFloatValue_json(root, ConstValues::A_HEIGHT.c_str());
    
    if ( PARSER->checkObjectExist_json(root, ConstValues::TRANSFORM.c_str()) )
    {
        parseTransform(PARSER->getSubDictionary_json(root, ConstValues::TRANSFORM.c_str()), ellipseData->transform);
        parsePivot(PARSER->getSubDictionary_json(root, ConstValues::TRANSFORM.c_str()), ellipseData->pivot);
    }
    
    return ellipseData;
}

void JSONDataParser::parseTimeline(const rapidjson::Value &root, Timeline &timeline) const
{
    int position = 0;
    Frame *frame = nullptr;
    
    for (size_t i = 0, l = timeline.frameList.size(); i < l; ++i)
    {
        frame = timeline.frameList[i];
        frame->position = position;
        position += frame->duration;
    }
    
    if (frame)
    {
        frame->duration = timeline.duration - frame->position;
    }
}

void JSONDataParser::parseFrame(const rapidjson::Value &root, Frame &frame) const
{
    frame.duration = (int)(round(PARSER->getIntValue_json(root, ConstValues::A_DURATION.c_str()) * 1000.f / _frameRate));
    
    if (PARSER->checkObjectExist_json(root, ConstValues::A_ACTION.c_str()))
    {
        frame.action = PARSER->getStringValue_json(root, ConstValues::A_ACTION.c_str());
    }
    
    if (PARSER->checkObjectExist_json(root, ConstValues::A_EVENT.c_str()))
    {
        frame.event = PARSER->getStringValue_json(root, ConstValues::A_EVENT.c_str());
    }
    
    if (PARSER->checkObjectExist_json(root, ConstValues::A_SOUND.c_str()))
    {
        frame.sound = PARSER->getStringValue_json(root, ConstValues::A_SOUND.c_str());
    }
}

void JSONDataParser::parseTransform(const rapidjson::Value &root, Transform &transform) const
{
    transform.x = PARSER->getFloatValue_json(root, ConstValues::A_X.c_str()) / _armatureScale;
    transform.y = PARSER->getFloatValue_json(root, ConstValues::A_Y.c_str()) / _armatureScale;
    transform.skewX = PARSER->getFloatValue_json(root, ConstValues::A_SKEW_X.c_str()) * ANGLE_TO_RADIAN;
    transform.skewY = PARSER->getFloatValue_json(root, ConstValues::A_SKEW_Y.c_str()) * ANGLE_TO_RADIAN;
    transform.scaleX = PARSER->getFloatValue_json(root, ConstValues::A_SCALE_X.c_str());
    transform.scaleY = PARSER->getFloatValue_json(root, ConstValues::A_SCALE_Y.c_str());
}

void JSONDataParser::parsePivot(const rapidjson::Value &root, Point &pivot) const
{
    pivot.x = PARSER->getFloatValue_json(root, ConstValues::A_PIVOT_X.c_str()) / _armatureScale;
    pivot.y = PARSER->getFloatValue_json(root, ConstValues::A_PIVOT_Y.c_str()) / _armatureScale;
}

void JSONDataParser::parseColorTransform(const rapidjson::Value &root, ColorTransform &colorTransform) const
{
    colorTransform.alphaOffset = PARSER->getIntValue_json(root, ConstValues::A_ALPHA_OFFSET.c_str());
    colorTransform.redOffset = PARSER->getIntValue_json(root, ConstValues::A_RED_OFFSET.c_str());
    colorTransform.greenOffset = PARSER->getIntValue_json(root, ConstValues::A_GREEN_OFFSET.c_str());
    colorTransform.blueOffset = PARSER->getIntValue_json(root, ConstValues::A_BLUE_OFFSET.c_str());
    colorTransform.alphaMultiplier = PARSER->getFloatValue_json(root, ConstValues::A_ALPHA_MULTIPLIER.c_str()) * 0.01f;
    colorTransform.redMultiplier = PARSER->getFloatValue_json(root, ConstValues::A_RED_MULTIPLIER.c_str()) * 0.01f;
    colorTransform.greenMultiplier = PARSER->getFloatValue_json(root, ConstValues::A_GREEN_MULTIPLIER.c_str()) * 0.01f;
    colorTransform.blueMultiplier = PARSER->getFloatValue_json(root, ConstValues::A_BLUE_MULTIPLIER.c_str()) * 0.01f;
}

void JSONDataParser::parseTextData(const rapidjson::Value &root, TextData &textData) const
{
    textData.bold = getBoolean(root, ConstValues::A_BOLD.c_str(), false);
    textData.italic = getBoolean(root, ConstValues::A_ITALIC.c_str(), false);
    
    textData.size = PARSER->getIntValue_json(root, ConstValues::A_SIZE.c_str());
    
    if ( PARSER->checkObjectExist_json(root, ConstValues::COLOR.c_str()) )
    {
        const rapidjson::Value &color = PARSER->getSubDictionary_json(root, ConstValues::COLOR.c_str());
        textData.alpha =  PARSER->getIntValue_json(color, ConstValues::A_ALPHA.c_str());
        textData.red =  PARSER->getIntValue_json(color, ConstValues::A_RED.c_str());
        textData.green =  PARSER->getIntValue_json(color, ConstValues::A_GREEN.c_str());
        textData.blue =  PARSER->getIntValue_json(color, ConstValues::A_BLUE.c_str());
    }
    
    textData.width = PARSER->getIntValue_json(root, ConstValues::A_WIDTH.c_str());
    textData.height = PARSER->getIntValue_json(root, ConstValues::A_HEIGHT.c_str());
    
    textData.face = PARSER->getStringValue_json(root, ConstValues::A_FACE.c_str());
    textData.text = PARSER->getStringValue_json(root, ConstValues::A_TEXT.c_str());
    
    textData.alignH = getAlignHType(PARSER->getStringValue_json(root, ConstValues::A_ALIGN_H.c_str()));
    textData.alignV = getAlignVType(PARSER->getStringValue_json(root, ConstValues::A_ALIGN_V.c_str()));
}
NAME_SPACE_DRAGON_BONES_END
