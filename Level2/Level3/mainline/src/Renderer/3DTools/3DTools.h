// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DTOOLS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DTOOLS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DTOOLS_EXPORTS
#define DTOOLS_API __declspec(dllexport)
#else
#define DTOOLS_API __declspec(dllimport)
#endif


// This class is exported from the 3DTools.dll
namespace Ogre
{
	class Vector3;
}
namespace C3DTools
{

	
	class DTOOLS_API C3DS2Mesh
	{
	public:
		C3DS2Mesh(const char* meshPrefix, const char* matPrefix, float scale =1.0f);

		float mScale;
		const char* mMeshPrefix;
		const char* mMaterailPrefix;
		bool Convert(const char* szFile, const char* desFolder, Ogre::Vector3& dim );
	};

};

