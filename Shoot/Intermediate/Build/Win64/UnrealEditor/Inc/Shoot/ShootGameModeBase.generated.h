// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef SHOOT_ShootGameModeBase_generated_h
#error "ShootGameModeBase.generated.h already included, missing '#pragma once' in ShootGameModeBase.h"
#endif
#define SHOOT_ShootGameModeBase_generated_h

#define FID_Shoot_Source_Shoot_ShootGameModeBase_h_15_SPARSE_DATA
#define FID_Shoot_Source_Shoot_ShootGameModeBase_h_15_RPC_WRAPPERS
#define FID_Shoot_Source_Shoot_ShootGameModeBase_h_15_RPC_WRAPPERS_NO_PURE_DECLS
#define FID_Shoot_Source_Shoot_ShootGameModeBase_h_15_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAShootGameModeBase(); \
	friend struct Z_Construct_UClass_AShootGameModeBase_Statics; \
public: \
	DECLARE_CLASS(AShootGameModeBase, AGameModeBase, COMPILED_IN_FLAGS(0 | CLASS_Transient | CLASS_Config), CASTCLASS_None, TEXT("/Script/Shoot"), NO_API) \
	DECLARE_SERIALIZER(AShootGameModeBase)


#define FID_Shoot_Source_Shoot_ShootGameModeBase_h_15_INCLASS \
private: \
	static void StaticRegisterNativesAShootGameModeBase(); \
	friend struct Z_Construct_UClass_AShootGameModeBase_Statics; \
public: \
	DECLARE_CLASS(AShootGameModeBase, AGameModeBase, COMPILED_IN_FLAGS(0 | CLASS_Transient | CLASS_Config), CASTCLASS_None, TEXT("/Script/Shoot"), NO_API) \
	DECLARE_SERIALIZER(AShootGameModeBase)


#define FID_Shoot_Source_Shoot_ShootGameModeBase_h_15_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AShootGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AShootGameModeBase) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AShootGameModeBase); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AShootGameModeBase); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AShootGameModeBase(AShootGameModeBase&&); \
	NO_API AShootGameModeBase(const AShootGameModeBase&); \
public:


#define FID_Shoot_Source_Shoot_ShootGameModeBase_h_15_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AShootGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AShootGameModeBase(AShootGameModeBase&&); \
	NO_API AShootGameModeBase(const AShootGameModeBase&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AShootGameModeBase); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AShootGameModeBase); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AShootGameModeBase)


#define FID_Shoot_Source_Shoot_ShootGameModeBase_h_12_PROLOG
#define FID_Shoot_Source_Shoot_ShootGameModeBase_h_15_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Shoot_Source_Shoot_ShootGameModeBase_h_15_SPARSE_DATA \
	FID_Shoot_Source_Shoot_ShootGameModeBase_h_15_RPC_WRAPPERS \
	FID_Shoot_Source_Shoot_ShootGameModeBase_h_15_INCLASS \
	FID_Shoot_Source_Shoot_ShootGameModeBase_h_15_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define FID_Shoot_Source_Shoot_ShootGameModeBase_h_15_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Shoot_Source_Shoot_ShootGameModeBase_h_15_SPARSE_DATA \
	FID_Shoot_Source_Shoot_ShootGameModeBase_h_15_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Shoot_Source_Shoot_ShootGameModeBase_h_15_INCLASS_NO_PURE_DECLS \
	FID_Shoot_Source_Shoot_ShootGameModeBase_h_15_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> SHOOT_API UClass* StaticClass<class AShootGameModeBase>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Shoot_Source_Shoot_ShootGameModeBase_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
