#include "TVNode.h"

CArchive& operator>>(CArchive& ar, CTVNode::EState& eState)
{
	int val;
	ar.Read(&val, sizeof(val) );
	switch(val)
	{
	case 1:
		eState = CTVNode::collapsed;
		break;
	case 2:
		eState = CTVNode::expanded;
		break;
	}
	return ar;
}

CArchive& operator<<(CArchive& ar, CTVNode::EState& eState)
{
	int val = 0;
	switch(eState)
	{
	case CTVNode::collapsed:
		val = 1;
		break;
	case CTVNode::expanded:
		val = 2;
		break;
	}
	ar.Write(&val, sizeof(val) );
	return ar;
}