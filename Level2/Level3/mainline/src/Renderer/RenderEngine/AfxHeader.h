#pragma once
#include <string>
//
namespace Ogre
{


	typedef unsigned char uchar;
	typedef unsigned short ushort;
	typedef unsigned int uint;
	typedef unsigned long ulong;

	typedef unsigned int uint32;
	typedef unsigned short uint16;
	typedef unsigned char uint8;


	class Angle;
	class Animation;
	class AnimationState;
	class AnimationStateSet;
	class AnimationTrack;
	class Archive;
	class ArchiveFactory;
	class ArchiveManager;
	class AutoParamDataSource;
	class AxisAlignedBox;
	class AxisAlignedBoxSceneQuery;
	class Billboard;
	class BillboardChain;
	class BillboardSet;
	class Bone;
	class Camera;
	class Codec;
	class ColourValue;
	class ConfigDialog;
	template <typename T> class Controller;
	template <typename T> class ControllerFunction;
	class ControllerManager;
	template <typename T> class ControllerValue;
	class Degree;
	class DynLib;
	class DynLibManager;
	class EdgeData;
	class EdgeListBuilder;
	class Entity;
	class ErrorDialog;
	class ExternalTextureSourceManager;
	class Factory;
	class Font;
	class FontPtr;
	class FontManager;
	struct FrameEvent;
	class FrameListener;
	class Frustum;
	class GpuProgram;
	class GpuProgramPtr;
	class GpuProgramManager;
	class GpuProgramUsage;
	class HardwareIndexBuffer;
	class HardwareOcclusionQuery;
	class HardwareVertexBuffer;
	class HardwarePixelBuffer;
	class HardwarePixelBufferSharedPtr;
	class HighLevelGpuProgram;
	class HighLevelGpuProgramPtr;
	class HighLevelGpuProgramManager;
	class HighLevelGpuProgramFactory;
	class IndexData;
	class IntersectionSceneQuery;
	class IntersectionSceneQueryListener;
	class Image;
	class KeyFrame;
	class Light;
	class Log;
	class LogManager;
	class ManualResourceLoader;
	class ManualObject;
	class Material;
	class MaterialPtr;
	class MaterialManager;
	class Math;
	class Matrix3;
	class Matrix4;
	class MemoryManager;
	class Mesh;
	class MeshPtr;
	class MeshSerializer;
	class MeshSerializerImpl;
	class MeshManager;
	class MovableObject;
	class MovablePlane;
	class Node;
	class NodeAnimationTrack;
	class NodeKeyFrame;
	class NumericAnimationTrack;
	class NumericKeyFrame;
	class Overlay;
	class OverlayContainer;
	class OverlayElement;
	class OverlayElementFactory;
	class OverlayManager;
	class Particle;
	class ParticleAffector;
	class ParticleAffectorFactory;
	class ParticleEmitter;
	class ParticleEmitterFactory;
	class ParticleSystem;
	class ParticleSystemManager;
	class ParticleSystemRenderer;
	class ParticleSystemRendererFactory;
	class ParticleVisualData;
	class Pass;
	class PatchMesh;
	class PixelBox;
	class Plane;
	class PlaneBoundedVolume;
	class Plugin;
	class Pose;
	class ProgressiveMesh;
	class Profile;
	class Profiler;
	class Quaternion;
	class Radian;
	class Ray;
	class RaySceneQuery;
	class RaySceneQueryListener;
	class Renderable;
	class RenderPriorityGroup;
	class RenderQueue;
	class RenderQueueGroup;
	class RenderQueueInvocation;
	class RenderQueueInvocationSequence;
	class RenderQueueListener;
	class RenderSystem;
	class RenderSystemCapabilities;
	class RenderSystemCapabilitiesManager;
	class RenderSystemCapabilitiesSerializer;
	class RenderTarget;
	class RenderTargetListener;
	class RenderTexture;
	class MultiRenderTarget;
	class RenderWindow;
	class RenderOperation;
	class Resource;
	class ResourceBackgroundQueue;
	class ResourceGroupManager;
	class ResourceManager;
	class RibbonTrail;
	class Root;
	class SceneManager;
	class SceneManagerEnumerator;
	class SceneNode;
	class SceneQuery;
	class SceneQueryListener;
	class ScriptCompiler;
	class ScriptCompilerManager;
	class ScriptLoader;
	class Serializer;
	class ShadowCaster;
	class ShadowRenderable;
	class ShadowTextureManager;
	class SimpleRenderable;
	class SimpleSpline;
	class Skeleton;
	class SkeletonPtr;
	class SkeletonInstance;
	class SkeletonManager;
	class Sphere;
	class SphereSceneQuery;
	class StaticGeometry;
	class StringConverter;
	class StringInterface;
	class SubEntity;
	class SubMesh;
	class TagPoint;
	class Technique;
	class TempBlendedBufferInfo;
	class ExternalTextureSource;
	class TextureUnitState;
	class Texture;
	class TexturePtr;
	class TextureManager;
	class TransformKeyFrame;
	class Timer;
	class UserDefinedObject;
	class Vector2;
	class Vector3;
	class Vector4;
	class Viewport;
	class VertexAnimationTrack;
	class VertexBufferBinding;
	class VertexData;
	class VertexDeclaration;
	class VertexMorphKeyFrame;
	class WireBoundingBox;
	class Compositor;
	class CompositorManager;
	class CompositorChain;
	class CompositorInstance;
	class CompositionTechnique;
	class CompositionPass;
	class CompositionTargetPass;
	class Any;
	typedef std::string  String;
};
//preserved visible flags
//arc ext class;
namespace Ogre
{
	class ManualObjectExt;
    class BillboardObject;
}
#define SelectableFlag			1
#define NormalVisibleFlag		2
#define SelectableFirstFlag		4
#define SnapCameraVisibleFlag	8
#define ProcessorTagVisibleFlag	16

typedef size_t SelectID;

//#ifndef __in
//#define __in
//#endif
//#ifndef __out
//#define __out
//#endif
//#ifndef __inout
//#define __inout
//#endif
//#ifndef __in_opt
//#define __in_opt
//#endif
//#ifndef __out_opt
//#define __out_opt
//#endif
//#ifndef __inout_opt
//#define __inout_opt
//#endif
//#ifndef __in_ecount
//#define __in_ecount(x)
//#endif
//#ifndef __out_ecount
//#define __out_ecount(x)
//#endif
//#ifndef __inout_ecount
//#define __inout_ecount(x)
//#endif
//#ifndef __in_bcount
//#define __in_bcount(x)
//#endif
//#ifndef __out_bcount
//#define __out_bcount(x)
//#endif
//#ifndef __inout_bcount
//#define __inout_bcount(x)
//#endif
//#ifndef __out_xcount
//#define __out_xcount(x)
//#endif
//#ifndef __deref_opt_out
//#define __deref_opt_out
//#endif
//#ifndef __deref_out
//#define __deref_out
//#endif
//#ifndef __out_ecount_opt
//#define __out_ecount_opt(x)
//#endif
//#ifndef __in_bcount_opt
//#define __in_bcount_opt(x)
//#endif
//#ifndef __out_bcount_opt
//#define __out_bcount_opt(x)
//#endif
//#ifndef __deref_out_opt
//#define __deref_out_opt
//#endif
//#ifndef __in_z
//#define __in_z
//#endif
//#ifndef __in_z_opt
//#define __in_z_opt
//#endif
//
//#define __in_ecount(THING) /**/
//#define __in_ecount_opt(THING) /**/
//#define __out_ecount(THING) /**/
//#define __out_ecount_opt(THING) /**/
//#define __inout_ecount(THING) /**/
//#define __inout_ecount_opt(THING) /**/
//
//#define __in_bcount_opt(THING) /**/
//#define __out_bcount_opt(THING) /**/
//#define __inout_bcount_opt(THING) /**/
//
//#define WINAPI_INLINE inline
//
//
//
//
