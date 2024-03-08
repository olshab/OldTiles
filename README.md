# Old Tiles Development

There are three stages:
1. **Uncooking** both live and old tiles
2. **Merging** them together fixing all collision mismatches
3. **Injecting** merged tiles into live tiles

# Blueprint Generation specifics

## Child Actors
One of the components that tiles can contain is **ChildActorComponents**, which is a way to have a blueprint inside of another blueprint (for example, *Shack Structure* blueprint inside of *Shack tile*). So these child actor blueprints must be generated too, before the creation of tile blueprint.
To choose a blueprint for child component, set it in **ChildActorClass** property.

## Actor Spawners
Tiles also contain ActorSpawners - special components, that can spawn blueprints *only if certain conditions are met*.
ActorSpawers have three main properties:
* **ActivatedSceneElement** - determines which blueprint will be spawned if that ActorSpawner is marked as Activated
* **DeactivatedSceneElement** - determines which blueprint will be spawned if that ActorSpawner is marked as Deactivated
* **Visualization** - used only in Editor to visualize that ActorSpawner in viewport (*not used in game*)

Most of the ActorSpawners have only **ActivatedSceneElement**, whereas **DeactivatedSceneElement** is empty. This means if that ActorSpawner is **Deactivated**, nothing will be spawned (for example, *basement* in *Shack tile*). But some ActorSpawners has **DeactivatedSceneElement** as well, like *shack floor* in *Shack tile* - if it's activated (which means basement is spawned in that tile as well), then floor variation with an access to the basement will be spawned, otherwise there's gonna be spawned just plain floor.

So, ActorSpawner is not a static object that always exists in tile, and we shouldn't create it in our tile. But my tool generates tiles with them just for the purpose of visualization. All ActorSpawners in the project are created as child actors (with the blueprint from **Visualization** property) and have "*ActorSpawner*" tag. All ActorSpawners will be stripped out during the process of injection tile blueprints.

# 1. Finding Blueprints Referenced in Tiles

Tile Blueprints can reference another blueprints, for example this Asylum tile references `FenceEdge_Asy_01`.
It means that blueprint is used as Child Actor in tile:

![ReferenceTree](https://github.com/olshab/OldTiles/blob/main/Guide/ReferenceTree.png?raw=true)

So if you uncook tile blueprint without uncooking `FenceEdge_Asy_01` first, you will end up with empty Child Actor when you generate tile blueprint, because there is no such `FenceEdge_Asy_01` in the project yet. To avoid that, you need to uncook `FenceEdge_Asy_01` first and then the tile blueprint `BP_TL_Bd_16x16_Fence01_Asy` itself. 

To check what blueprints are referenced in tiles, you need to use **BlueprintReferenceViewer** tool:

![BlueprintReferenceViewerUI](https://github.com/olshab/OldTiles/blob/main/Guide/BlueprintReferenceViewerUI.png?raw=true)

First of all, export all tile `.uasset` blueprints for particular map, for example, all assets at `DeadByDaylight/Content/Blueprints/Tiles/04-Asylum`, and move them all to a separate folder, on screenshot above that folder is `C:\Users\Oleg\Desktop\Assets`

Set all other settings, for example if you are uncooking live tiles, set `Paks Directory` to live Paks folder and `Engine Version` to 4.27

Also you need to provide a path to OldTiles UE project. If you'll do that, **BlueprintReferenceViewer** will be aware of already existing blueprint in your project. As you can see, in the output window all blueprints that already exist in the project marked with grey:

![ReferenceExistingAssets](https://github.com/olshab/OldTiles/blob/main/Guide/ReferenceExistingAssets.png?raw=true)

Since both live and old tiles usually have blueprints with the same name (`BP_MediumTree01` exists in 3.0.0 and in 7.5.0), you need to specify which blueprints you need to account when BlueprintReferenceViewer will look for existing blueprints. This can be done with this list:

![IgnoreExistingAssetsAtPath](https://github.com/olshab/OldTiles/blob/main/Guide/IgnoreExistingAssetsAtPath.png?raw=true)

1. `/Game/MergedTiles` - should be always here
2. `/Game/NewTiles` - if you are uncooking old 3.0.0 tiles/blueprints
3. `/Game/OriginalTiles` - if you are uncooking live tiles/blueprints

After you click **Run**, in `Assets Folder` will appear the following folders:

![AssetsFolder](https://github.com/olshab/OldTiles/blob/main/Guide/AssetsFolder.png?raw=true)

Now you need to uncook blueprints contained in these folders starting from the highest level (`Level 1` in my case) up to the `Initial Blueprints`.

# 2. Uncooking Tiles/Blueprints

This is done in two steps: 
1. Dumping `.uasset` blueprints as `.json` with **BlueprintDumper** 
2. Generating blueprint asset from `.json` dump in editor with **BlueprintUncooker** UE plugin.

**BlueprintDumper** window looks like this:

![BlueprintDumperUI](https://github.com/olshab/OldTiles/blob/main/Guide/BlueprintDumperUI.png?raw=true)

After you've dumped blueprints, you need to generate them in editor. Open `BlueprintUncooker` window:

![BlueprintUncookerUI](https://github.com/olshab/OldTiles/blob/main/Guide/BlueprintUncookerUI.png?raw=true)

`Path To Dumped JSON Files` should be the same as `Assets Folder` in **BlueprintDumper**, as well as `Generate at Path`

After you click `Generate` button, assets will appear:
- Blueprints at `Content/[New/Original]Tiles/Blueprints/_buffer`
- Meshes at `Content/[New/Original]Tiles/Meshes/_buffer`
- Materials at `Content/[New/Original]Tiles/Materials/_buffer`
- Textures at ``Content/[New/Original]Tiles/Textures/_buffer``

# 3. Merging Tiles/ActorSpawners in Editor

**BlueprintMergeTool** is accessible from Top Menu -> Window tab:

![BlueprintMergeTool](https://github.com/olshab/OldTiles/blob/main/Guide/BlueprintMergeTool.png?raw=true)

Merging process works as follows: a copy of the blueprint specified as the `Destination` *(from NewTiles)* is created and all components from the blueprint specified as the `Source` *(from OriginalTiles)* are copied to it.

With that:
- Materials of all StaticMesh components in Destination blueprint are replaced with `Translucent Material` (semi-transparent blue by default) if `Make Meshes Translucent` set to True. Note that it will not affect meshes that are inside of ChildActor components
- All components from Destination blueprint are set to be `HiddenInGame` (but they are still visible in Editor) and `NewTiles` component tag is add to them. **Warning:** every time you want to use something from Destination blueprint (for example, Rocks meshes), don't forget to set `HiddenInGame` to false and clear `NewTiles` tag. All components with `NewTiles` are stripped out during injecting process
- All `ActorSpawners` (which are actually just ChildActors) from OriginalTiles are hidden in both Editor and Game, since ActorSpawners from 3.0.0 tiles are not used in live tiles in any ways
- All `ActorSpawners` from Destination blueprint (from NewTiles) are replaced with blueprints from `Folder To Take ActorSpawners From` if `Replace Actor Spawners` is set to True. ActorSpawners in merged blueprint exist only for visualization purposes, they are not used in game
