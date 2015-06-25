#pragma once 
class CAutoSaveHandlerInterface ;
#ifndef CAUTO_SAVE_INTERFACE_H 
  #define CAUTO_SAVE_INTERFACE_H
 
class CAutoSaveHandlerInterface 
{
public:
	 static CAutoSaveHandlerInterface* CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE ;
	CAutoSaveHandlerInterface();
	~CAutoSaveHandlerInterface() ;
public:
	virtual BOOL DoAutoSave() = 0 ;
};
#endif