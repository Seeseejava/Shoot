// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Shoot/ShootGameModeBase.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeShootGameModeBase() {}
// Cross Module References
	SHOOT_API UClass* Z_Construct_UClass_AShootGameModeBase_NoRegister();
	SHOOT_API UClass* Z_Construct_UClass_AShootGameModeBase();
	ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
	UPackage* Z_Construct_UPackage__Script_Shoot();
// End Cross Module References
	void AShootGameModeBase::StaticRegisterNativesAShootGameModeBase()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AShootGameModeBase);
	UClass* Z_Construct_UClass_AShootGameModeBase_NoRegister()
	{
		return AShootGameModeBase::StaticClass();
	}
	struct Z_Construct_UClass_AShootGameModeBase_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AShootGameModeBase_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AGameModeBase,
		(UObject* (*)())Z_Construct_UPackage__Script_Shoot,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AShootGameModeBase_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * \n */" },
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering HLOD WorldPartition DataLayers Transformation" },
		{ "IncludePath", "ShootGameModeBase.h" },
		{ "ModuleRelativePath", "ShootGameModeBase.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_AShootGameModeBase_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AShootGameModeBase>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_AShootGameModeBase_Statics::ClassParams = {
		&AShootGameModeBase::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x009002ACu,
		METADATA_PARAMS(Z_Construct_UClass_AShootGameModeBase_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_AShootGameModeBase_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AShootGameModeBase()
	{
		if (!Z_Registration_Info_UClass_AShootGameModeBase.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AShootGameModeBase.OuterSingleton, Z_Construct_UClass_AShootGameModeBase_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_AShootGameModeBase.OuterSingleton;
	}
	template<> SHOOT_API UClass* StaticClass<AShootGameModeBase>()
	{
		return AShootGameModeBase::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(AShootGameModeBase);
	struct Z_CompiledInDeferFile_FID_Shoot_Source_Shoot_ShootGameModeBase_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Shoot_Source_Shoot_ShootGameModeBase_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_AShootGameModeBase, AShootGameModeBase::StaticClass, TEXT("AShootGameModeBase"), &Z_Registration_Info_UClass_AShootGameModeBase, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AShootGameModeBase), 2792722236U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Shoot_Source_Shoot_ShootGameModeBase_h_813031425(TEXT("/Script/Shoot"),
		Z_CompiledInDeferFile_FID_Shoot_Source_Shoot_ShootGameModeBase_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Shoot_Source_Shoot_ShootGameModeBase_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
