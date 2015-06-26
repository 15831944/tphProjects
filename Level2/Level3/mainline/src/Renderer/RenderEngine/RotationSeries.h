#pragma once

#include <vector>
//#include "../Ogre/OgreNode.h"
#include ".\afxheader.h"

using namespace Ogre;
class RotationSeries
{
public:
	RENDERENGINE_API RotationSeries(size_t nReservedSeriesCount = 0);
	RENDERENGINE_API ~RotationSeries(void);

	struct RotationItem
	{
		RotationItem(const Vector3& _axis, Radian _angle, Node::TransformSpace _relativeTo = Node::TS_LOCAL);

		Vector3 axis;
		Radian angle;
		Node::TransformSpace relativeTo;
	};

	RENDERENGINE_API void AddRotation(const Vector3& axis, Radian angle, Node::TransformSpace relativeTo = Node::TS_LOCAL);

	void ApplyRotation(Node* pNode) const;

	class RotationApplier
	{
	public:
		RotationApplier(Node* pNode)
			: m_pNode(pNode)
		{

		}

		void operator() (const RotationItem& rot) const
		{
			m_pNode->rotate(rot.axis, rot.angle, rot.relativeTo);
		}

	private:
		Node* m_pNode;
	};
private:
	typedef std::vector<RotationItem> RotSeries;

	RotSeries m_rots;
};

