// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the RENDERENGINE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// RENDERENGINE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#pragma once

#include "AfxHeader.h"

#include "RenderDevice.h"

class RENDERENGINE_API RenderEngine
{
public:
	RenderEngine();
	~RenderEngine();

	void SetResourcePath(const CString& ResLoc); //need to call this, when app start; the root paht 

	Ogre::RenderWindow* CreateRenderWindow(HWND hwnd);
	void DestroyRenderWindow(Ogre::RenderWindow* pwnd);

	Ogre::SceneManager* CreateSceneManager();
	void DestroySceneManager(Ogre::SceneManager* pSm);

	static RenderEngine& getSingleton();
	INT_PTR ShowConfigDlg();	// return IDOK or IDCANCEL
	bool GetAlwaysInUse() const { return m_bAlwaysInUse; }
	void SetAlwaysInUse(bool bAlwaysInUse) { m_bAlwaysInUse = bAlwaysInUse; }
	bool IsCurrentOpenGL()const; //return current is opengl render system
	bool IsCurrentDirectX()const; //return is current is dx render system

	
	static HINSTANCE GetModule();
	
	static Ogre::RenderWindow* getPrimeWindow();
	bool MakeSureLoaded();
	bool MakeSurePrimRenderWndCreated();
	bool IsLoaded()const;
protected:
	bool TryShutDown(); //manual shut down render engine,make sure the scene and window are destroyed;
protected:
	bool Load();
	void UnLoad();

	bool bInUse()const;

	//main render window 
    bool CreatePrimRenderWindow();
	void DestroyPrimWnd();	
	bool IsPrimRenderWindowCreated()const;

	//resources relate to render window
	void InitExt();
	void DestroyExt();
	void LoadConfigResource(); //load resource from config file

private:
	bool m_bAlwaysInUse; // use the render engine always, 3DView setting related
};

#include <boost/shared_ptr.hpp>

class RENDERENGINE_API DataBuffer
{
public:
	typedef boost::shared_ptr<DataBuffer> SharedPtr;
	size_t nsize;
	LPVOID lpData; 

	DataBuffer(size_t dsize){ lpData = (LPVOID)::GlobalAlloc(GMEM_FIXED, dsize);  nsize = dsize; }
	~DataBuffer(){ ::GlobalFree((HGLOBAL)lpData); }
};

//for external resource files
class RENDERENGINE_API ResourceFileArchive
{
public:
	static void AddZipFile(CString zipFile, CString groupName, bool bForce = true );
	static void AddDirectory(CString dir, CString groupName, bool bForce = true);
	static void RemoveLocation(CString location,CString groupName);

	static void LoadConfigResource(CString configfile);

	static CString ReadFileAsString(CString fileName,CString groupName);
	static DataBuffer::SharedPtr ReadFileAsBuffer(CString fileName, CString groupname); 
};