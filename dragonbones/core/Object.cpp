#include "Object.h"
#include "Armature.h"

NAME_SPACE_DRAGON_BONES_BEGIN
bool Object::getVisible() const
{
    return _visible;
}
void Object::setVisible(bool visible)
{
    _visible = visible;
}

Armature* Object::getArmature() const
{
    return _armature;
}
Bone* Object::getParent() const
{
    return _parent;
}

void Object::setArmature(Armature *armature)
{
    if (_armature)
    {
        _armature->removeObject(this);
    }
    
    _armature = armature;
    
    if (_armature)
    {
        _armature->addObject(this);
    }
}
void Object::setParent(Bone *bone)
{
    _parent = bone;
}

Object::Object()
{
    _visible = true;
    _armature = nullptr;
    _parent = nullptr;
    offset.scaleX = offset.scaleY = 1.f;
    userData = nullptr;

	inheritRotation = true;
	inheritScale = true;
	inheritTranslation = true;
}
Object::~Object()
{
    dispose();
}
void Object::dispose()
{
    _armature = nullptr;
    _parent = nullptr;
    
    if (userData)
    {
        delete userData;
        userData = nullptr;
    }
}

void Object::calculateRelativeParentTransform() 
{

}

bool Object::calculateParentTransform(Transform &parentGlobalTransform, Matrix &parentGlobalTransformMatrix)
{
	if(_parent && (inheritTranslation || inheritRotation || inheritScale))
	{
		parentGlobalTransform = *(_parent->_globalTransformForChild);
		parentGlobalTransformMatrix = *(_parent->_globalTransformMatrixForChild);
				
		if(!inheritTranslation || !inheritRotation || !inheritScale)
		{
			parentGlobalTransform = Object::_tempParentGlobalTransform;

			parentGlobalTransform.copy(_parent->_globalTransformForChild);
			if(!inheritTranslation)
			{
				parentGlobalTransform.x = 0;
				parentGlobalTransform.y = 0;
			}
			if(!inheritScale)
			{
				parentGlobalTransform.scaleX = 1;
				parentGlobalTransform.scaleY = 1;
			}
			if(!inheritRotation)
			{
				parentGlobalTransform.skewX = 0;
				parentGlobalTransform.skewY = 0;
			}
					
			parentGlobalTransformMatrix = Object::_tempParentGlobalTransformMatrix;
			parentGlobalTransform.toMatrix(parentGlobalTransformMatrix, true);
		}
		return true;
	}
	return false;
}

bool Object::updateGlobal(Transform &parentGlobalTransform, Matrix &parentGlobalTransformMatrix)
{
	calculateRelativeParentTransform();
	global.toMatrix(globalTransformMatrix, true);
			
	if(calculateParentTransform(parentGlobalTransform, parentGlobalTransformMatrix))
	{
		globalTransformMatrix.concat(parentGlobalTransformMatrix);
		globalTransformMatrix.toTransform(global, global.scaleX * parentGlobalTransform.scaleX >= 0, global.scaleY * parentGlobalTransform.scaleY >= 0);
		return true;
	}
	return false;
}

NAME_SPACE_DRAGON_BONES_END