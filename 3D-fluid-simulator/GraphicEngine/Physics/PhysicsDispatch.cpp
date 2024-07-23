#include "PhysicsDispatch.hpp"

PhysicsDispatch& PhysicsDispatch::GetDispatchInstance() const
{
	static PhysicsDispatch instance;
	return instance;
}

void PhysicsDispatch::InitDefaultState()
{
}

void PhysicsDispatch::Reset()
{
}

void PhysicsDispatch::GenerateFrameForces() const
{
}

void PhysicsDispatch::AddListener(std::function<bool()> const& listenerCallback)
{
}
