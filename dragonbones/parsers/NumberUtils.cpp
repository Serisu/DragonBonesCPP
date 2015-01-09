#include "NumberUtils.h"

NAME_SPACE_DRAGON_BONES_BEGIN
float NumberUtils::toFixed(float number, int fixed){
	return ((round)(number * 10 * fixed)) / (10.0f * fixed);
}

NAME_SPACE_DRAGON_BONES_END