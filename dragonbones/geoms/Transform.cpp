#include "Transform.h"
#include "parsers/NumberUtils.h"

NAME_SPACE_DRAGON_BONES_BEGIN

Matrix *Transform::_helpTransformMatrix = new Matrix();
Matrix *Transform::_helpParentTransformMatrix = new Matrix();

void Transform::toMatrix(Matrix &matrix, bool keepScale) const
{
    if (keepScale)
    {
        matrix.a = scaleX * cos(skewY);
        matrix.b = scaleX * sin(skewY);
        matrix.c = -scaleY * sin(skewX);
        matrix.d = scaleY * cos(skewX);
    }
    else
    {
        matrix.a = cos(skewY);
        matrix.b = sin(skewY);
        matrix.c = -sin(skewX);
        matrix.d = cos(skewX);
    }
        
    matrix.tx = x;
    matrix.ty = y;
}

void Transform::globalToLocal(const Transform &parent)
{
	this->toMatrix(*_helpTransformMatrix, true);
	parent.toMatrix(*_helpParentTransformMatrix, true);
			
	_helpParentTransformMatrix->invert();
	_helpTransformMatrix->concat(*_helpParentTransformMatrix);
			
	matrixToTransform(*_helpTransformMatrix, *this, this->scaleX * parent.scaleX >= 0, this->scaleY * parent.scaleY >= 0);
}

void Transform::transformWith(const Transform &parent)
{
	this->toMatrix(*Transform::_helpTransformMatrix, true);
	parent.toMatrix(*Transform::_helpParentTransformMatrix, true);
		
	_helpParentTransformMatrix->invert();
	_helpTransformMatrix->concat(*_helpParentTransformMatrix);
			
	_helpTransformMatrix->toTransform(*this, this->scaleX * parent.scaleX >= 0, this->scaleY * parent.scaleY >= 0);
		
}


NAME_SPACE_DRAGON_BONES_END