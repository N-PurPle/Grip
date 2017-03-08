#pragma once

#define GRIP_UNUSED(x)


namespace Grip {

template<typename T>
void SafeRelease(T*& ptr)
{
	if (ptr != nullptr)
	{
		ptr->Release();
		ptr = nullptr;
	}
}

} // namespace Grip


