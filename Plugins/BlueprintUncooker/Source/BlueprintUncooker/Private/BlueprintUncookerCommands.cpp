// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlueprintUncookerCommands.h"

#define LOCTEXT_NAMESPACE "FBlueprintUncookerModule"

void FBlueprintUncookerCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "BlueprintUncooker", "Bring up BlueprintUncooker window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
