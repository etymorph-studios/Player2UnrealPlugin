# READ ME!

## About
This plugin is designed to simplify the integration of the Player2 SDK within Unreal Engine projects. It provides a straightforward, drop-and-play solution, abstracting the complexities of the SDK and allowing game developers to focus on their core gameplay.

## Example

A full guide is WIP. However there is a demo level and assets found inside the plugin context folder. These components showcase how to use the plugin.

## BUILD

Place plugin inside your projects plugin folder ->  "ProjectName/Plugin/Player2UnrealPlugin"

Once there, you can recompile your project by right clicking your solution, "ProjectPath/ProjectName.uproject". Click the option that says "Generate Visual Studio project files". From there just compile and build your game as you would normally do.

Open up your game from unreal engine editor. Click "Edit -> Plugins". Click "Project" from the left toolbar menu. Apply "Player2" and restart the editor. Now your game will have Player2 capabilities. 

## PREREQUISITE

Player2 requires a Game Client ID, this can be found inside the [Developer Portal](https://player2.game/profile/developer). Once you have your game client ID, you must stick it inside the "HeaderPlayer2GameKey" config option found inside the game settings: Edit -> Project Settings -> Player2 Settings -> Header Player 2Game Key.

## License
Read License

