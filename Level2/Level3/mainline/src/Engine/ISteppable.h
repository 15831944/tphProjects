#pragma once
class CARCportEngine;

class ISteppable
{
public:
	virtual void step(CARCportEngine& state)=0;
};
