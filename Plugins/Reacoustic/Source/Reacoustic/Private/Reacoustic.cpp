// Copyright Epic Games, Inc. All Rights Reserved.

#include "Reacoustic.h"

#include "ReacousticSubsystem.h"

/** Log categories for Reacoustic. */
DEFINE_LOG_CATEGORY(LogReacousticSubsystem)
DEFINE_LOG_CATEGORY(LogReacousticComponent)



void FReacousticModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogTemp, Error, TEXT("Reacoustic Startupmodule Triggered"));
}







void FReacousticModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FReacousticModule, Reacoustic)