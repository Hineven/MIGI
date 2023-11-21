#pragma once
namespace C
{
	// 1MB Shared buffer size by default.
	constexpr size_t DefaultSharedBufferSize = 4 * 1024 * 1024;
	constexpr int C::ThreadGroupSize1D = 128;
	constexpr int C::ThreadGroupSize2D = 16;
	constexpr int NNInputWidth = 4;
	constexpr int NNOutputWidth = 4;
}
