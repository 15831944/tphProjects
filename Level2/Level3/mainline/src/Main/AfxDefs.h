#ifdef _AFXDLL
#define IMPLEMENT_RUNTIMECLASS_EXT(class_name, full_class_name, base_class_name, wSchema, pfnNew) \
	CRuntimeClass* PASCAL full_class_name::_GetBaseClass() \
		{ return RUNTIME_CLASS(base_class_name); } \
	AFX_COMDAT const AFX_DATADEF CRuntimeClass full_class_name::class##class_name = { \
		#full_class_name, sizeof(class full_class_name), wSchema, pfnNew, \
			&full_class_name::_GetBaseClass, NULL }; \
	CRuntimeClass* full_class_name::GetRuntimeClass() const \
		{ return RUNTIME_CLASS(class_name); } \

#define _IMPLEMENT_RUNTIMECLASS_EXT(class_name, full_class_name, base_class_name, wSchema, pfnNew) \
	CRuntimeClass* PASCAL full_class_name::_GetBaseClass() \
		{ return RUNTIME_CLASS(base_class_name); } \
	AFX_COMDAT AFX_DATADEF CRuntimeClass full_class_name::class##class_name = { \
		#full_class_name, sizeof(class full_class_name), wSchema, pfnNew, \
			&full_class_name::_GetBaseClass, NULL }; \
	CRuntimeClass* full_class_name::GetRuntimeClass() const \
		{ return RUNTIME_CLASS(class_name); } \

#else
#define IMPLEMENT_RUNTIMECLASS_EXT(class_name, full_class_name, base_class_name, wSchema, pfnNew) \
	AFX_COMDAT const AFX_DATADEF CRuntimeClass full_class_name::class##class_name = { \
		#full_class_name, sizeof(class full_class_name), wSchema, pfnNew, \
			RUNTIME_CLASS(base_class_name), NULL }; \
	CRuntimeClass* full_class_name::GetRuntimeClass() const \
		{ return RUNTIME_CLASS(class_name); } \

#define _IMPLEMENT_RUNTIMECLASS_EXT(class_name, full_class_name, base_class_name, wSchema, pfnNew) \
	AFX_DATADEF CRuntimeClass full_class_name::class##class_name = { \
		#full_class_name, sizeof(class full_class_name), wSchema, pfnNew, \
			RUNTIME_CLASS(base_class_name), NULL }; \
	CRuntimeClass* full_class_name::GetRuntimeClass() const \
		{ return RUNTIME_CLASS(class_name); } \

#endif

#define IMPLEMENT_DYNAMIC_EXT(class_name, full_class_name, base_class_name) \
	IMPLEMENT_RUNTIMECLASS_EXT(class_name, full_class_name, base_class_name, 0xFFFF, NULL)