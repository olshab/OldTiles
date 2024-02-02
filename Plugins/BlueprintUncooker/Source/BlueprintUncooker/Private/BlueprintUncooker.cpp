// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlueprintUncooker.h"
#include "BlueprintUncookerStyle.h"
#include "BlueprintUncookerCommands.h"
#include "ToolMenus.h"

#include "BlueprintUncookerWidget.h"

static const FName BlueprintUncookerTabName("BlueprintUncooker");

#define LOCTEXT_NAMESPACE "FBlueprintUncookerModule"

void FBlueprintUncookerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FBlueprintUncookerStyle::Initialize();
	FBlueprintUncookerStyle::ReloadTextures();

	FBlueprintUncookerCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FBlueprintUncookerCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FBlueprintUncookerModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FBlueprintUncookerModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(BlueprintUncookerTabName, FOnSpawnTab::CreateRaw(this, &FBlueprintUncookerModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FBlueprintUncookerTabTitle", "Blueprint Uncooker"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FBlueprintUncookerModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);

	FBlueprintUncookerStyle::Shutdown();
	FBlueprintUncookerCommands::Unregister();
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(BlueprintUncookerTabName);
}

TSharedRef<SDockTab> FBlueprintUncookerModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return
		SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SBlueprintUncookerWidget)
		];
}

void FBlueprintUncookerModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(BlueprintUncookerTabName);
}

void FBlueprintUncookerModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FBlueprintUncookerCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FBlueprintUncookerCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBlueprintUncookerModule, BlueprintUncooker)