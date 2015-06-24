#pragma once


class PathFinderListener
{
public:
	PathFinderListener(){};
	virtual ~PathFinderListener(){};

	// Virtual Callbacks
public:
	virtual void OnPathFound(const std::vector<int>& thePath) = 0;

};