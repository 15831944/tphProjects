#include "stdafx.h"
#include "CAutoSaveHandlerInterface.h"
CAutoSaveHandlerInterface* CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE = NULL;
CAutoSaveHandlerInterface::CAutoSaveHandlerInterface(){
	CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE = this ;
};
CAutoSaveHandlerInterface::~CAutoSaveHandlerInterface()
{ 
	CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE = NULL;
} ;
