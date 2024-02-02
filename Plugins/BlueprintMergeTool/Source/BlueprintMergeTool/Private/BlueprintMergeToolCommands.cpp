// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlueprintMergeToolCommands.h"

#define LOCTEXT_NAMESPACE "FBlueprintMergeToolModule"

void FBlueprintMergeToolCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "BlueprintMergeTool", "Bring up BlueprintMergeTool window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
