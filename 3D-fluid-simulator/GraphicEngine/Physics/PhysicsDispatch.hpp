#pragma once

#include "Program.h"
#include "Properties.h"
#include "ShaderStorageBuffer.h"
#include <cstdint>
#include <functional>


class PhysicsDispatch{
private:
	Program _forcesQueue;
	uint64_t _timestamp;
private:
	PhysicsDispatch();
public:
	PhysicsDispatch& GetDispatchInstance() const;
	void InitDefaultState();
	void Reset();
	void GenerateFrameForces() const;
	void AddListener(std::function<bool()> const& listenerCallback);
};
