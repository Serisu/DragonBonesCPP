#ifndef GEOMS_MATRIX_H
#define GEOMS_MATRIX_H

#include "../DragonBones.h"
#include "Point.h"
#include "NumberUtils.h"
#include "Transform.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class Matrix
{
public:
    float a;
    float b;
    float c;
    float d;
    float tx;
    float ty;
    
public:
    Matrix()
    {
        a = 0.f;
        b = 0.f;
        c = 0.f;
        d = 0.f;
        tx = 0.f;
        ty = 0.f;
    }
    Matrix(const Matrix &copyData)
    {
        operator=(copyData);
    }
    void operator=(const Matrix &copyData)
    {
        a = copyData.a;
        b = copyData.b;
        c = copyData.c;
        d = copyData.d;
        tx = copyData.tx;
        ty = copyData.ty;
    }
    virtual ~Matrix() {}
    
    void invert()
    {
        const float a0 = a;
        const float b0 = b;
        const float c0 = c;
        const float d0 = d;
        const float tx0 = tx;
        const float ty0 = ty;
        const float determinant = 1 / (a0 * d0 - b0 * c0);
        a = determinant * d0;
        b = -determinant * b0;
        c = -determinant * c0;
        d = determinant * a0;
        tx = determinant * (c0 * ty0 - d0 * tx0);
        ty = determinant * (b0 * tx0 - a0 * ty0);
    }
    
    void transformPoint(Point &point)
    {
        float x = point.x;
        float y = point.y;
        point.x = a * x + c * y + tx;
        point.y = d * y + b * x + ty;
    }

	void concat(Matrix &m) 
	{
        if (m.a != 1 || m.b != 0 || m.c != 0 || m.d != 1) {
            a = a * m.a + b * m.c;
            b = a * m.b + b * m.d;
            c = c * m.a + d * m.c;
            d = c * m.b + d * m.d;
        }
		tx = tx * m.a + ty * m.c + m.tx;
        ty = tx * m.b + ty * m.d + m.ty;
	}

	void toTransform(Transform &transform, bool scaleXF, bool scaleYF)
		{
			transform.x = this->tx;
			transform.y = this->ty;
			transform.scaleX = sqrt(this->a * this->a + this->b * this->b) * (scaleXF ? 1 : -1);
			transform.scaleY = sqrt(this->d * this->d + this->c * this->c) * (scaleYF ? 1 : -1);
			
			float skewXArray[4];
			skewXArray[0] = acos(this->d / transform.scaleY);
			skewXArray[1] = -skewXArray[0];
			skewXArray[2] = asin(-this->c / transform.scaleY);
			skewXArray[3] = skewXArray[2] >= 0 ? PI - skewXArray[2] : skewXArray[2] - PI;
			
			if(NumberUtils::toFixed(skewXArray[0], 4) == NumberUtils::toFixed(skewXArray[2], 4)
				|| NumberUtils::toFixed(skewXArray[0], 4) == NumberUtils::toFixed(skewXArray[3], 4))
			{
				transform.skewX = skewXArray[0];
			}
			else 
			{
				transform.skewX = skewXArray[1];
			}
			
			float skewYArray[4];
			skewYArray[0] = acos(this->a / transform.scaleX);
			skewYArray[1] = -skewYArray[0];
			skewYArray[2] = asin(this->b / transform.scaleX);
			skewYArray[3] = skewYArray[2] >= 0 ? PI - skewYArray[2] : skewYArray[2] - PI;

			if(NumberUtils::toFixed(skewYArray[0], 4) == NumberUtils::toFixed(skewYArray[2], 4)
				|| NumberUtils::toFixed(skewYArray[0], 4) == NumberUtils::toFixed(skewYArray[3], 4))
			{
				transform.skewY = skewYArray[0];
			}
			else 
			{
				transform.skewY = skewYArray[1];
			}
			
		}
};
NAME_SPACE_DRAGON_BONES_END
#endif  // GEOMS_MATRIX_H