// RenderEngine.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "resource.h"
#include "RenderEngine.h"

#include <RenderSystems\GL\OgreGLPlugin.h>
#include <Plugins\CgProgramManager\OgreCgPlugin.h>
#include <RenderSystems\Direct3D9\OgreD3D9Plugin.h>
//#include <renderSystems\Direct3D10\OgreD3D10Plugin.h>
//#include <renderSystems\Direct3D11\OgreD3D11Plugin.h>
//
#include "SelectionManager.h"
#include "DlgRenderOptions.h"
#include "ManualObjectExt.h"
//#include "LayerManualObject.h"
#include "OgreBillboardObject.h"
#include "./ColoredTextAreaOverlayElement.h"
#include <common\TmpFileManager.h>
#define Max(a,b) (a>b)?(a):(b)


static AFX_EXTENSION_MODULE RendererDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{		

		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(RendererDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(RendererDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		// Terminate the library before destructor are called
		AfxTermExtensionModule(RendererDLL);
	}
	return 1;   // ok
}



using namespace Ogre;

LPCSTR dummyText = "RenderDummy";
CString mResourceRootPath = ".";
#define PrimeWindow "PrimaryWindow"


HWND m_hwnd = NULL;//hidden window
Ogre::RenderWindow* prenderwnd = NULL;//prime render window 
int nCreateWndCount = 0; //sub render window created


RenderEngine::RenderEngine()
: m_bAlwaysInUse(false)
{

}

RenderEngine::~RenderEngine()
{
	UnLoad();
}


void RenderEngine::UnLoad()
{
	delete SelectionManager::getSingletonPtr();
	//SelectionManager::getSingleton().Clear();
	DestroyPrimWnd();
	delete Root::getSingletonPtr();		

    if(m_hwnd)
	{
		DestroyWindow(m_hwnd);
		UnregisterClass(dummyText, NULL);
		m_hwnd = NULL;
	}
}

bool RenderEngine::IsPrimRenderWindowCreated()const
{
	return prenderwnd !=NULL;
}

void RenderEngine::LoadConfigResource()
{		
	if(!IsLoaded())
		return;

	CString resourceCfgFile;
	resourceCfgFile.Format(_T("%s\\resources.cfg"), mResourceRootPath.GetString());
	ResourceFileArchive::LoadConfigResource(resourceCfgFile);
}


void RenderEngine::SetResourcePath( const CString& ResLoc )
{
	ASSERT(!IsPrimRenderWindowCreated());
	mResourceRootPath = ResLoc;
}
//
//const static CString sLoadplugins[] = 
//{
//	_T("RenderSystem_Direct3D9_d"),	
//};
//
//bool LoadPlugin(const CString& sdll)
//{
//	try
//	{
//		Root::getSingleton().loadPlugin(sdll.GetString());
//	}	
//	catch (Exception&)
//	{
//		return false;
//	}
//	return true;
//}

bool RenderEngine::Load()
{
	UnLoad();
	TCHAR path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	*strrchr(path,'\\') = '\0';
    
	CString strLog ; strLog.Format("%s\\renderer.log",path);
	CString strCfg ; strCfg.Format("%s\\renderer.cfg",path);
	CString pluginFold; pluginFold.Format("%s\\opt\\", path);
	Root* pRoot = new Root("",strCfg.GetString(),strLog.GetString());	
	new SelectionManager();

	GLPlugin* mGLPlugin = new GLPlugin();
	pRoot->installPlugin(mGLPlugin);

	D3D9Plugin* mD3D9Plugin = new D3D9Plugin();
	pRoot->installPlugin(mD3D9Plugin);	


	//D3D10Plugin* mD3D10Plugin = new D3D10Plugin();
	//pRoot->installPlugin(mD3D10Plugin);

	//D3D11Plugin* mD3D11Plugin = new D3D11Plugin();
	//pRoot->installPlugin(mD3D11Plugin);


	CgPlugin* mCgPlugin = new CgPlugin;
	pRoot->installPlugin(mCgPlugin);

	try
	{
		pRoot->restoreConfig();	
	}	
	catch (...)
	{
		::DeleteFile(strCfg);
		delete pRoot;
		Load();
	}
	
	
	if(!pRoot->getRenderSystem())//render system not configured right
	{
		Ogre::RenderSystemList rslist =  pRoot->getAvailableRenderers();
		if(rslist.size())
		{
			pRoot->setRenderSystem(*rslist.begin());	
		}	
	}

	return pRoot!=NULL;
}

void RenderEngine::DestroyPrimWnd()
{
	if(!IsPrimRenderWindowCreated())
		return;

	ASSERT(nCreateWndCount==0);
	nCreateWndCount =0;	
	DestroyExt();	

	Root::getSingleton().getRenderSystem()->destroyRenderWindow(PrimeWindow);
	prenderwnd = NULL;
}

void GetConfigOption(NameValuePairList& parms)
{
	ConfigOptionMap& conf =  Root::getSingleton().getRenderSystem()->getConfigOptions();
	for(ConfigOptionMap::iterator itr=conf.begin();itr!=conf.end();++itr)
		parms[itr->second.name] = itr->second.currentValue;
}

Ogre::RenderWindow* RenderEngine::CreateRenderWindow( HWND hwnd )
{
	if(!MakeSurePrimRenderWndCreated())
		return NULL;


	Ogre::RenderWindow* pNewWnd = NULL;
	try
	{
		NameValuePairList parms;
		GetConfigOption(parms);
		parms["externalWindowHandle"] = StringConverter::toString((long)hwnd);
		pNewWnd = Root::getSingleton().createRenderWindow(StringConverter::toString((long)hwnd), 0,0 , false, &parms);
		nCreateWndCount++;
	}	
	catch (Exception&)
	{
		
	}
	return pNewWnd;
}

bool RenderEngine::CreatePrimRenderWindow()
{
	if(!MakeSureLoaded())
	{
		return false;
	}
	Root* pRoot = Root::getSingletonPtr();
	try
	{
		pRoot->initialise(false);

		WNDCLASS dummyClass;
		memset(&dummyClass, 0, sizeof(WNDCLASS));
		dummyClass.style = CS_OWNDC;
		dummyClass.hInstance = NULL;
		dummyClass.lpfnWndProc = DefWindowProc;
		dummyClass.lpszClassName = dummyText;
		RegisterClass(&dummyClass);
		m_hwnd = CreateWindow(dummyText, dummyText,
			WS_POPUP | WS_CLIPCHILDREN,
			0, 0, 1, 1, 0, 0, NULL, 0);

		
		NameValuePairList parms;
		//GetConfigOption(parms);
		parms["externalWindowHandle"] = StringConverter::toString((long)m_hwnd);	
		prenderwnd = pRoot->createRenderWindow(PrimeWindow, 0,0, false, &parms);
		InitExt();
	}
	catch (Exception&)
	{
		if(IDOK == ShowConfigDlg())
		{
			return CreatePrimRenderWindow();
		}
	}
	return prenderwnd!=NULL;
}



void RenderEngine::DestroyRenderWindow( Ogre::RenderWindow* pwnd )
{
	if(!Root::getSingletonPtr())
		return ;

	if(!Root::getSingleton().getRenderSystem())
		return;

	Root::getSingleton().getRenderSystem()->destroyRenderWindow(pwnd->getName());
	nCreateWndCount--;

	TryShutDown();
}

bool RenderEngine::MakeSureLoaded()
{
	if(IsLoaded())return true;	
	return Load();
}

bool RenderEngine::IsLoaded() const
{
	return Root::getSingletonPtr()!=NULL ;
}

bool RenderEngine::MakeSurePrimRenderWndCreated()
{
	if(!MakeSureLoaded())
		return false;

	if(IsPrimRenderWindowCreated())
		return true;
	return CreatePrimRenderWindow();
}

RenderEngine& RenderEngine::getSingleton()
{
	static RenderEngine inst;
	return inst;
}

Ogre::SceneManager* RenderEngine::CreateSceneManager()
{
	if( !MakeSurePrimRenderWndCreated() )
		return NULL;		
	SceneManager* pScen = NULL;
	try
	{	
		pScen = Root::getSingleton().createSceneManager(ST_GENERIC);
	}	
	catch (Exception&)
	{
	}
	return pScen;

}

void RenderEngine::DestroySceneManager( Ogre::SceneManager* pSm )
{
	if(!IsLoaded())
		return;

	Root::getSingleton().destroySceneManager(pSm);
	TryShutDown();
}

bool RenderEngine::TryShutDown()
{
	if(!IsLoaded())
		return false;
	
	if(bInUse())
		return false;

	UnLoad();
	return true;
}

bool RenderEngine::bInUse() const
{
	SceneManagerEnumerator::SceneManagerIterator iter = Root::getSingleton().getSceneManagerIterator();
	if(iter.hasMoreElements())
		return true;
	
	if(nCreateWndCount>0)
		return true;

	return false;
}

HINSTANCE RenderEngine::GetModule()
{
	return RendererDLL.hModule;
}



void RenderEngine::InitExt()
{
	SelectionManager::getSingleton().Clear();
	Root::getSingleton().addMovableObjectFactory(new ManualObjectExtFactory());
	Root::getSingleton().addMovableObjectFactory(new BillboardObjectFactory());
	//Root::getSingleton().addMovableObjectFactory(new LayerManualObjectFactory());

	//OverlayManager::getSingleton().addOverlayElementFactory(new ColoredTextAreaOverlayElementFactory() );
	LoadConfigResource();

	Root::getSingleton().getRenderSystem()->setNormaliseNormals(true);
}

void RenderEngine::DestroyExt()
{
	
}

INT_PTR RenderEngine::ShowConfigDlg()
{
	MakeSureLoaded();
	if (IsPrimRenderWindowCreated())
	{
		AfxMessageBox(_T("Please close all 3D views first!"));
		return IDCANCEL;
	}
	CDlgRenderOptions dlg(m_bAlwaysInUse);
	INT_PTR nRet = dlg.DoModal();
	if (IDOK == nRet)
	{
		m_bAlwaysInUse = dlg.IsUseNewRenderEngine();
	}
	return nRet;
}

bool RenderEngine::IsCurrentOpenGL() const
{
	if(IsLoaded())
	{
		return Root::getSingleton().getRenderSystem()->getName() == "OpenGL Rendering Subsystem";
	}
	return false;
}

bool RenderEngine::IsCurrentDirectX() const
{
	if(IsLoaded())
	{
		return Root::getSingleton().getRenderSystem()->getName() == "Direct3D9 Rendering Subsystem";
	}
	return false;
}

Ogre::RenderWindow* RenderEngine::getPrimeWindow()
{
	return prenderwnd;
}



void ResourceFileArchive::AddZipFile( CString zipFile, CString groupName, bool bForce /*= false */ )
{
	if(!RenderEngine::getSingleton().IsLoaded())
		return;	
	
	ResourceGroupManager::getSingleton().addResourceLocation(
		zipFile.GetString(),"Zip" , groupName.GetString());
	ResourceGroupManager::getSingleton().initialiseResourceGroup(zipFile.GetString());	

}


void ResourceFileArchive::AddDirectory( CString dir, CString groupName, bool bForce /*= false*/ )
{
	if(!RenderEngine::getSingleton().IsLoaded())
		return;

	ResourceGroupManager::getSingleton().addResourceLocation(
		dir.GetString(),"FileSystem", groupName.GetString());
	ResourceGroupManager::getSingleton().initialiseResourceGroup(groupName.GetString());

}

CString ResourceFileArchive::ReadFileAsString( CString fileName,CString groupName )
{
	CString ret;
	if(RenderEngine::getSingleton().IsLoaded())
	{
		DataStreamPtr dsp = ResourceGroupManager::getSingleton().openResource(fileName.GetString(),groupName.GetString());
		if(!dsp.isNull())
			return dsp->getAsString().c_str();
	}
	return ret;
}

DataBuffer::SharedPtr ResourceFileArchive::ReadFileAsBuffer( CString fileName, CString groupName )
{
	DataBuffer* pNewBuffer =  NULL;
	if(RenderEngine::getSingleton().IsLoaded())
	{
		DataStreamPtr dsp = ResourceGroupManager::getSingleton().openResource(fileName.GetString(),groupName.GetString());
		if(dsp->size())
		{
			pNewBuffer = new DataBuffer(dsp->size());
			dsp->read(pNewBuffer->lpData,pNewBuffer->nsize);
		}
	}
	return DataBuffer::SharedPtr(pNewBuffer);
}

void ResourceFileArchive::RemoveLocation( CString location,CString groupName )
{
	if(!RenderEngine::getSingleton().IsLoaded())
		return;
	ResourceGroupManager::getSingleton().removeResourceLocation(
		location.GetString(), groupName.GetString());

}

void ResourceFileArchive::LoadConfigResource( CString configfile )
{
	Ogre::ConfigFile cf;
	cf.load(configfile.GetString());
	CString configfilePath = configfile.Left(configfile.ReverseFind('\\')+1);

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
	Ogre::String secName, typeName, archName;

	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		bool bAdded = false;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;

			CString strArchname = configfilePath+archName.c_str();
			//strArchname.Format(_T("%s\\%s"),mResourceRootPath.GetString(), archName.c_str());

			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				strArchname.GetString(), typeName, secName);
			bAdded = true;
		}
		if(bAdded)
			Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(secName);

	}
}