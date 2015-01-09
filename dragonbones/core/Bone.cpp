#include "Bone.h"

NAME_SPACE_DRAGON_BONES_BEGIN
bool Bone::sortState(const TimelineState *a, const TimelineState *b)
{
    return a->_animationState->getLayer() < b->_animationState->getLayer();
}

Slot* Bone::getSlot() const
{
    return _slotList.empty() ? nullptr : _slotList.front();
}

const std::vector<Slot*>& Bone::getSlots() const
{
    return _slotList;
}

const std::vector<Bone*>& Bone::getBones() const
{
    return _boneList;
}

void Bone::setVisible(bool visible)
{
    if (_visible != visible)
    {
        _visible = visible;
        
        for (size_t i = 0, l = _slotList.size(); i < l; ++i)
        {
            _slotList[i]->updateDisplayVisible(_visible);
        }
    }
}

void Bone::setArmature(Armature *armature)
{
    Object::setArmature(armature);
    
    for (size_t i = 0, l = _boneList.size(); i < l; ++i)
    {
        _boneList[i]->setArmature(armature);
    }
    
    for (size_t i = 0, l = _slotList.size(); i < l; ++i)
    {
        _slotList[i]->setArmature(armature);
    }
}

Bone::Bone()
{
    _isColorChanged = false;
    _needUpdate = 2;
    _tween.scaleX = _tween.scaleY = 1.f;
	
    
	applyOffsetTranslationToChild = true;
	applyOffsetRotationToChild = true;
	applyOffsetScaleToChild = false;
}
Bone::~Bone()
{
    dispose();
}
void Bone::dispose()
{
    Object::dispose();
    _boneList.clear();
    _slotList.clear();
    _timelineStateList.clear();
}

void Bone::invalidUpdate()
{
    _needUpdate = 2;
}

bool Bone::contains(const Object *object) const
{
    if (!object)
    {
        // throw
    }
    
    if (object == this)
    {
        return false;
    }
    
    const Object *ancestor = object;
    
    while (!(ancestor == this || ancestor == nullptr))
    {
        ancestor = ancestor->getParent();
    }
    
    return ancestor == this;
}

void Bone::addChild(Object *object)
{
    if (!object)
    {
        // throw
    }
    
    Bone *bone = dynamic_cast<Bone*>(object);
    Slot *slot = dynamic_cast<Slot*>(object);
    
    if (object == this || (bone && bone->contains(this)))
    {
        DBASSERT(false, "An Bone cannot be added as a child to itself or one of its children (or children's children, etc.)");
    }
    
    if (object && object->getParent())
    {
        object->getParent()->removeChild(object);
    }
    
    if (bone)
    {
        _boneList.push_back(bone);
        bone->setParent(this);
        bone->setArmature(_armature);
    }
    else if (slot)
    {
        _slotList.push_back(slot);
        slot->setParent(this);
        slot->setArmature(_armature);
    }
}

void Bone::removeChild(Object *object)
{
    if (!object)
    {
        // throw
    }
    
    Bone *bone = dynamic_cast<Bone*>(object);
    Slot *slot = dynamic_cast<Slot*>(object);
    
    if (bone)
    {
        auto iterator = std::find(_boneList.begin(), _boneList.end(), bone);
        
        if (iterator != _boneList.end())
        {
            _boneList.erase(iterator);
            bone->setParent(nullptr);
            bone->setArmature(nullptr);
        }
        else
        {
            // throw
        }
    }
    else if (slot)
    {
        auto iterator = std::find(_slotList.begin(), _slotList.end(), slot);
        
        if (iterator != _slotList.end())
        {
            _slotList.erase(iterator);
            slot->setParent(nullptr);
            slot->setArmature(nullptr);
        }
        else
        {
            // throw
        }
    }
}

void Bone::calculateRelativeParentTransform() 
{
	global.scaleX = origin.scaleX * _tween.scaleX * offset.scaleX;
	global.scaleY = origin.scaleY * _tween.scaleY * offset.scaleY;
	global.skewX = origin.skewX + _tween.skewX + offset.skewX;
	global.skewY = origin.skewY + _tween.skewY + offset.skewY;
	global.x = origin.x + _tween.x + offset.x;
	global.y = origin.y + _tween.y + offset.y;
}

void Bone::update(bool needUpdate)
{
    _needUpdate --;
    
    if (needUpdate || _needUpdate > 0 || (_parent && _parent->_needUpdate > 0))
    {
        _needUpdate = 1;
    }
    else
    {
        return;
    }
    
    blendingTimeline();


	//计算global
	Transform parentGlobalTransform;
	Matrix parentGlobalTransformMatrix;
	bool result = updateGlobal(parentGlobalTransform, parentGlobalTransformMatrix);
			
	//计算globalForChild
	bool ifExistOffsetTranslation = offset.x != 0 || offset.y != 0;
	bool ifExistOffsetScale = offset.scaleX != 0 || offset.scaleY != 0;
	bool ifExistOffsetRotation = offset.skewX != 0 || offset.skewY != 0;
			
	if(	(!ifExistOffsetTranslation || applyOffsetTranslationToChild) &&
		(!ifExistOffsetScale || applyOffsetScaleToChild) &&
		(!ifExistOffsetRotation || applyOffsetRotationToChild))
	{
		_globalTransformForChild = &global;
		_globalTransformMatrixForChild = &globalTransformMatrix;
	}
	else
	{
		if(!_tempGlobalTransformForChild)
		{
			_tempGlobalTransformForChild = new Transform();
		}
		_globalTransformForChild = _tempGlobalTransformForChild;
				
		if(!_tempGlobalTransformMatrixForChild)
		{
			_tempGlobalTransformMatrixForChild = new Matrix();
		}
		_globalTransformMatrixForChild = _tempGlobalTransformMatrixForChild;
				
		_globalTransformForChild->x = origin.x + _tween.x;
		_globalTransformForChild->y = origin.y + _tween.y;
		_globalTransformForChild->scaleX = origin.scaleX * _tween.scaleX;
		_globalTransformForChild->scaleY = origin.scaleY * _tween.scaleY;
		_globalTransformForChild->skewX = origin.skewX + _tween.skewX;
		_globalTransformForChild->skewY = origin.skewY + _tween.skewY;
				
		if(applyOffsetTranslationToChild)
		{
			_globalTransformForChild->x += offset.x;
			_globalTransformForChild->y += offset.y;
		}
		if(applyOffsetScaleToChild)
		{
			_globalTransformForChild->scaleX *= offset.scaleX;
			_globalTransformForChild->scaleY *= offset.scaleY;
		}
		if(applyOffsetRotationToChild)
		{
			_globalTransformForChild->skewX += offset.skewX;
			_globalTransformForChild->skewY += offset.skewY;
		}
				
		_globalTransformForChild->toMatrix(*_globalTransformMatrixForChild, true);
		if(result)
		{
			_globalTransformMatrixForChild->concat(parentGlobalTransformMatrix);
			_globalTransformMatrixForChild->toTransform(*_globalTransformForChild, 
				_globalTransformForChild->scaleX * parentGlobalTransform.scaleX >= 0, 
				_globalTransformForChild->scaleY * parentGlobalTransform.scaleY >= 0 );
		}
	}
			

}

void Bone::updateColor(
    int aOffset,
    int rOffset,
    int gOffset,
    int bOffset,
    float aMultiplier,
    float rMultiplier,
    float gMultiplier,
    float bMultiplier,
    bool colorChanged
)
{
    for (size_t i = 0, l = _slotList.size(); i < l; ++i)
    {
        _slotList[i]->updateDisplayColor(
            aOffset, rOffset, gOffset, bOffset,
            aMultiplier, rMultiplier, gMultiplier, bMultiplier
        );
    }
    
    _isColorChanged = colorChanged;
}

void Bone::hideSlots()
{
    for (size_t i = 0, l = _slotList.size(); i < l; ++i)
    {
        _slotList[i]->changeDisplay(-1);
    }
}

void Bone::arriveAtFrame(TransformFrame *frame, const TimelineState *timelineState, AnimationState *animationState, bool isCross)
{
    // TODO:
    bool displayControl =
        animationState->displayControl &&
        (displayController.empty() || displayController == animationState->name);
        
    // && timelineState->_weight > 0
    // TODO: 需要修正混合动画干扰关键帧数据的问题，如何正确高效的判断混合动画？
    if (displayControl)
    {
        const int displayIndex = frame->displayIndex;
        
        for (size_t i = 0, l = _slotList.size(); i < l; ++i)
        {
            Slot *slot = _slotList[i];
            slot->changeDisplay(displayIndex);
            slot->updateDisplayVisible(frame->visible);
            
            if (displayIndex >= 0)
            {
                if (frame->zOrder != slot->_tweenZOrder)
                {
                    slot->_tweenZOrder = frame->zOrder;
                    _armature->_slotsZOrderChanged = true;
                }
            }
        }
        
        if (!frame->event.empty() && _armature->_eventDispatcher->hasEvent(EventData::EventType::BONE_FRAME_EVENT))
        {
            EventData *eventData = EventData::borrowObject(EventData::EventType::BONE_FRAME_EVENT);
            eventData->armature = _armature;
            eventData->bone = this;
            eventData->animationState = animationState;
            eventData->frameLabel = frame->event;
            eventData->frame = frame;
            _armature->_eventDataList.push_back(eventData);
        }
        
        if (!frame->sound.empty() && Armature::soundEventDispatcher && Armature::soundEventDispatcher->hasEvent(EventData::EventType::SOUND))
        {
            EventData *eventData = EventData::borrowObject(EventData::EventType::SOUND);
            eventData->armature = _armature;
            eventData->bone = this;
            eventData->animationState = animationState;
            eventData->sound = frame->sound;
            Armature::soundEventDispatcher->dispatchEvent(eventData);
        }
        
        if (!frame->action.empty())
        {
            for (size_t i = 0, l = _slotList.size(); i < l; ++i)
            {
                if (_slotList[i]->_childArmature)
                {
                    _slotList[i]->_childArmature->_animation->gotoAndPlay(frame->action);
                }
            }
        }
    }
}

void Bone::addState(TimelineState *timelineState)
{
    auto iterator = std::find(_timelineStateList.cbegin(), _timelineStateList.cend(), timelineState);
    
    if (iterator == _timelineStateList.cend())
    {
        _timelineStateList.push_back(timelineState);
        std::sort(_timelineStateList.begin() , _timelineStateList.end() , sortState);
    }
}

void Bone::removeState(TimelineState *timelineState)
{
    auto iterator = std::find(_timelineStateList.begin(), _timelineStateList.end(), timelineState);
    
    if (iterator != _timelineStateList.end())
    {
        _timelineStateList.erase(iterator);
    }
}

void Bone::blendingTimeline()
{
    size_t i = _timelineStateList.size();
    
    if (i == 1)
    {
        TimelineState *timelineState = _timelineStateList[0];
        const Transform &transform = timelineState->_transform;
        const Point &pivot = timelineState->_pivot;
        timelineState->_weight = timelineState->_animationState->getCurrentWeight();
        const float weight = timelineState->_weight;
        _tween.x = transform.x * weight;
        _tween.y = transform.y * weight;
        _tween.skewX = transform.skewX * weight;
        _tween.skewY = transform.skewY * weight;
        _tween.scaleX = 1 + (transform.scaleX - 1) * weight;
        _tween.scaleY = 1 + (transform.scaleY - 1) * weight;
        _tweenPivot.x = pivot.x * weight;
        _tweenPivot.y = pivot.y * weight;
    }
    else if (i > 1)
    {
        int prevLayer = _timelineStateList[i - 1]->_animationState->getLayer();
        int currentLayer = 0;
        float weigthLeft = 1.f;
        float layerTotalWeight = 0.f;
        float x = 0.f;
        float y = 0.f;
        float skewX = 0.f;
        float skewY = 0.f;
        float scaleX = 1.f;
        float scaleY = 1.f;
        float pivotX = 0.f;
        float pivotY = 0.f;
        
        while (i--)
        {
            TimelineState *timelineState = _timelineStateList[i];
            currentLayer = timelineState->_animationState->getLayer();
            
            if (prevLayer != currentLayer)
            {
                if (layerTotalWeight >= weigthLeft)
                {
                    timelineState->_weight = 0;
                    break;
                }
                else
                {
                    weigthLeft -= layerTotalWeight;
                }
            }
            
            prevLayer = currentLayer;
            timelineState->_weight = timelineState->_animationState->getCurrentWeight() * weigthLeft;
            const float weight = timelineState->_weight;
            
            //timelineState
            if (weight && timelineState->_blendEnabled)
            {
                const Transform &transform = timelineState->_transform;
                const Point &pivot = timelineState->_pivot;
                x += transform.x * weight;
                y += transform.y * weight;
                skewX += transform.skewX * weight;
                skewY += transform.skewY * weight;
                scaleX += (transform.scaleX - 1) * weight;
                scaleY += (transform.scaleY - 1) * weight;
                pivotX += pivot.x * weight;
                pivotY += pivot.y * weight;
                layerTotalWeight += weight;
            }
        }
        
        _tween.x = x;
        _tween.y = y;
        _tween.skewX = skewX;
        _tween.skewY = skewY;
        _tween.scaleX = scaleX;
        _tween.scaleY = scaleY;
        _tweenPivot.x = pivotX;
        _tweenPivot.y = pivotY;
    }
}
NAME_SPACE_DRAGON_BONES_END