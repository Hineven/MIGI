#pragma once
#include "DirectMLConfig.h"
#include "DirectML.h"
#include "d3d12.h"


class MIGIMLDevice
{
public:
	MIGIMLDevice();
	~MIGIMLDevice();

	void Initialize();
	void Shutdown();

	inline IDMLDevice* GetDevice() const { return Device.Get(); }
	inline ID3D12Resource* GetInputBuffer () const { return InputBuffer.Get();}
	inline ID3D12Resource* GetOutputBuffer () const { return OutputBuffer.Get();}
	void ExecuteNetwork () const ;
private:
	TUniquePtr<IDMLDevice> Device;
	TUniquePtr<ID3D12Resource> InputBuffer;
	TUniquePtr<ID3D12Resource> OutputBuffer;
};