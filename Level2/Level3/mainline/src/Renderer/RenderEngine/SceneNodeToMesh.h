#pragma once


namespace Ogre
{
	class SceneNodeToMesh : Renderable::Visitor
	{
	public:
		void Init(CString meshPrefix);
		void VisitNode(Ogre::SceneNode* pNode);
		void Save( CString folder);

	protected:
		std::vector<MaterialPtr> mMaterials;

		Ogre::String szMeshPrefix;

		Ogre::MeshPtr m_ogreMesh;
		std::map<String,String> mTexSource; //name source

		virtual void visit(Renderable* rend, ushort lodIndex, bool isDebug, 
			Any* pAny = 0);

		void SaveMaterial(const String& folder);
		void SaveMesh(const String& folder);
		void CopyTexures(const String& folder); //copy textures to new folder

		void AddMovable(Ogre::MovableObject* pObj);

		void visiteMaterial(MaterialPtr& mat);
		bool MirrorNode(const RenderOperation& rop,float * pCurPos,size_t numPoints)const;
		bool MirrorNode(const RenderOperation& rop,IndexData* pData)const;
	};

}
