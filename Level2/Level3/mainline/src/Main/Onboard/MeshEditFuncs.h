#pragma once


class CComponentEditContext;
class AddSectionCommand;
class EditSectionPropCommand;
class CComponentMeshSection;
class DelSectionCommand;

class MeshEditFuncs
{
public:
	static AddSectionCommand* OnAddSection(CComponentEditContext* pCtx,double dAtPos);
	static EditSectionPropCommand* OnEditSection(CComponentEditContext* pCtx,CComponentMeshSection* pSection);
	static DelSectionCommand* OnDelSection(CComponentEditContext* pCtx,CComponentMeshSection* pSection);
	static AddSectionCommand* OnCopySection(CComponentEditContext* pCtx,CComponentMeshSection* pSection);
};