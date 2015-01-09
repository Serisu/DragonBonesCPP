#ifndef GEOMS_TRANSFORM_H
#define GEOMS_TRANSFORM_H

#include "../DragonBones.h"
#include "Matrix.h"
NAME_SPACE_DRAGON_BONES_BEGIN
class Matrix;
class Transform
{
public:
    float x;
    float y;
    float skewX;
    float skewY;
    float scaleX;
    float scaleY;
    
	static Matrix *_helpTransformMatrix;
	static Matrix *_helpParentTransformMatrix;

public:
    float getRotation()
    {
        return skewX;
    }
    void setRotation(float value)
    {
        skewX = skewY = value;
    }
    
public:
    Transform()
    {
        x = 0.f;
        y = 0.f;
        skewX = 0.f;
        skewY = 0.f;
        scaleX = 1.f;
        scaleY = 1.f;

    }
    Transform(const Transform &copyData)
    {
        operator=(copyData);
    }
    Transform& operator=(const Transform &copyData)
    {
        x = copyData.x;
        y = copyData.y;
        skewX = copyData.skewX;
        skewY = copyData.skewY;
        scaleX = copyData.scaleX;
        scaleY = copyData.scaleY;
        return *this;
    }
    virtual ~Transform() {}
    
    void toMatrix(Matrix &matrix, bool keepScale = false) const;
    
    /*void transformWith(const Transform &parent)
    {
        Matrix matrix;
        parent.toMatrix(matrix, true);
        matrix.invert();
        const float x0 = x;
        const float y0 = y;
        x = matrix.a * x0 + matrix.c * y0 + matrix.tx;
        y = matrix.d * y0 + matrix.b * x0 + matrix.ty;
        skewX = formatRadian(skewX - parent.skewX);
        skewY = formatRadian(skewY - parent.skewY);
    }*/
	
    void transformWith(const Transform &parent);

	void globalToLocal(const Transform &parent);
	
	void matrixToTransform(Matrix &matrix, Transform &transform, bool scaleXF, bool scaleYF);
};
NAME_SPACE_DRAGON_BONES_END
#endif  // GEOMS_TRANSFORM_H
