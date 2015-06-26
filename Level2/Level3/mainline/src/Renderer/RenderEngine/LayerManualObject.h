#pragma once

namespace Ogre
{
	class LayerManualObject: public ManualObject
	{
	public:
		LayerManualObject(const String& name):ManualObject(name){}

		virtual void begin(const String& materialName,
			RenderOperation::OperationType opType = RenderOperation::OT_TRIANGLE_LIST, const String & groupName = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		class LayerManualObjectSection : public ManualObject::ManualObjectSection
		{
		public:
			LayerManualObjectSection(ManualObject* parent, const String& materialName,
				RenderOperation::OperationType opType, const String & groupName = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

			//most important
			Real getSquaredViewDepth(const Ogre::Camera *) const;
		};

	};


	/** Factory object for creating ManualObject instances */
	class _OgreExport LayerManualObjectFactory : public MovableObjectFactory
	{
	protected:
		MovableObject* createInstanceImpl( const String& name, const NameValuePairList* params);
	public:
		LayerManualObjectFactory() {}
		~LayerManualObjectFactory() {}

		static String FACTORY_TYPE_NAME;

		const String& getType(void) const;
		void destroyInstance( MovableObject* obj);  

	};
	/** @} */
	/** @} */

}

