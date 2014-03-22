#include "RenderSettings.h"

void RenderSettings::ClearScreen(Clearable * toClear, int nClearables)
{
	if (nClearables <= 0)
	{
		return;
	}

	GLenum finalEnum = ConvertEnum(toClear[0]);

	for (int i = 1; i < nClearables; ++i)
	{
		finalEnum |= ConvertEnum(toClear[i]);
	}
}