# Developer Documentation

This section provides an overview of the project's architecture and related resources.

## Architecture Diagram

To better understand the system's architecture, you can view the detailed diagram below:  

[here](./docs/public/R_type_architecture.pdf)  

The diagram outlines the key components and interactions within the project, showing how different modules and services communicate with each other.

## ECS

## Binary Protocol

Regarding communication between client and server, we use a binary protocol.
To achieve this, we defined a few basic commands so both client and server can communicate.
You can find all of them here:

[here](../docs/protocol_client_server.md)

## Stage Loader

The **stage loader** is a module that loads the stages from a config file in JSON format and creates the required entities.

Here how it works:

#### The config file
The config file is a JSON file that contains the information about the stages.

Here an example:
````
{
    "numero": 1,
    "background_path": "backgrounds/stage1.png",
    "music_path": "sounds/stage1.mp3",
    "time": 120,
    "seed": 42,
    "mobs_types": [
        "asteroid",
        "classic",
        "ship"
    ]
}
````
```numero```: design the stage number.  
```background_path```: the path to the background image.  
```music_path```: the path to the music file.  
```time```: the time in seconds that the stage will last.  
```seed```: the seed for the random informations generator.  
```mobs_types```: the types of mobs that will be spawned in the stage. Chosen between: *asteroid*, *classic*, *ship*.

#### The Loader
The logic of the loader is defined in the ```StageLoader``` class.
This class uses the *Singleton* pattern to ensure that only one instance of the loader is created. The method ```getInstance()``` is used to get the instance of the loader.

The loader contains the following methods, each with a specific purpose:
- ```loadConfig```: loads the config file and stores the information in a ```StageConfig``` object.
- ```genWaves```: generates the waves configuration (duration, mobs count, mobs type) for the stage.
- ```genMobsEntities```: add the mobs entities in the ECS's entities manager, based on the waves configuration.
These three methods are called successively to load the stage.

The method ```resetConfig``` is used to reset the loader's configuration to load a new stage.

#### The Loader System