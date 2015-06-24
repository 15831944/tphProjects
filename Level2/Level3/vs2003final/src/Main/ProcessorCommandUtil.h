#if !defined(_PROCESSORCOMMANDUTIL_H__INCLUDED_)
#define _PROCESSORCOMMANDUTIL_H__INCLUDED_


inline
static void FillProcIDVector(std::vector<UINT>& procidlist, CObjectDisplayList& proclist)
{
	procidlist.clear();
	procidlist.reserve(proclist.GetCount());

	int nCount = proclist.GetCount();
	for (int i =0; i < proclist.GetCount(); ++ i)
	{
		CObjectDisplay *pObjectDisplay = proclist.GetItem(i);
		if (pObjectDisplay && pObjectDisplay->GetType() == ObjectDisplayType_Processor2)
		{
			CProcessor2 *pProc2 = (CProcessor2 *)pObjectDisplay;

			procidlist.push_back(pProc2->GetSelectName() );


		}
	}

	//for(std::vector<CProcessor2*>::iterator it=proclist.begin(); it!=proclist.end(); ++it) {
	//	procidlist.push_back( (*it)->GetSelectName() );
	//}
}

inline
static void FillProc2Vector(std::vector<CProcessor2*>& proclist, std::vector<UINT>& procidlist, CTermPlanDoc* pDoc)
{
	proclist.clear();
	proclist.reserve(procidlist.size());
	for(std::vector<UINT>::iterator it=procidlist.begin(); it!=procidlist.end(); ++it) {
		CProcessor2* pProc2 = pDoc->GetProcWithSelName(*it);
		if(pProc2)
			proclist.push_back(pProc2);
	}
}

#endif