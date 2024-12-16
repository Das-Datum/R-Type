# R-Type

## The Project

### R-TYPE

This project is part of the Advanced C++ knowledge unit and is focused on networked video game development. You will be tasked with implementing a server and a graphical client for the iconic video game *R-Type*, using a game engine designed by you.

### Objective

The goal is to develop a networked version of *R-Type*, where players can join forces to fight the evil Bydos. The game will feature a server-client architecture, where the server manages the game state and the client displays the game and accepts player input.

### Game Overview

*R-Type* is a horizontal shoot 'em up (shmup) game that became immensely popular in the 1990s. It involves players controlling a spaceship, navigating through levels, and defeating enemy monsters called Bydos. Although *R-Type* is not the first game of its genre, it had a significant influence on the shmup genre, leading to several ports and remakes. In this project, you will recreate *R-Type* with the added complexity of networked multiplayer and enhanced game engine architecture.

By the end of this project, the game will not only be a fun multiplayer experience but also demonstrate your ability to build a game engine with scalable and maintainable architecture.

### Project Details

- **Binary**: `r-type_server`, `r-type_client`
- **Language**: C++
- **Build System**: CMake
- **Platforms**: Linux and Windows (cross-platform)
- **Networking**: Client-server architecture with real-time communication

This project will expose you to both the challenges of building a game engine and the intricacies of multiplayer networking, while promoting good software engineering practices and documentation standards.

## How to Start

To run this project, always use the run.sh script. The script automates the process of compiling and launching both the server and the client. Below are the different ways to use it:

### Server

To compile and start the server, use the following command:

```bash
./run.sh --server
```

This command will compile the necessary files and then launch the server.

### Client

To compile and start the client, use the following command:

```bash
./run.sh
```

### Running Without Compilation

If you don’t need to compile the application again, you can simply launch the already compiled client or server:

#### To launch the client without recompiling:

```bash
./run.sh --run
```

#### To launch the server without recompiling:

```bash
./run.sh --run --server
```

In summary, always use the run.sh script to compile and launch the client and/or server, or to run them without recompiling if you just want to start the already compiled applications.

## Protocol

The protocol documentation can be found [here](https://das-datum.github.io/R-Type/md_docs_protocol_client_server.html).  
It explains the communication rules and standards used in the project.

## Developer Documentation

The developer documentation is available [here](https://das-datum.github.io/R-Type/md_docs_dev_docs.html).  
This section provides information about the project structure, setup, and technical details for contributors.

## Comparative Study

The comparative study can be accessed [here](https://das-datum.github.io/R-Type/md_comparative_study.md).  
It discusses different approaches, technologies, and methodologies considered for this project.

## License

This project is licensed under the [GNU General Public License v3.0](./LICENSE.txt).  
See the LICENSE.txt file for details.

## Authors

<table style="width: 100%; table-layout: fixed;">
    <tbody>
        <tr>
            <td style="width: 20%; text-align: center;">
                <a href="https://github.com/Tristancalard"><img src="https://avatars.githubusercontent.com/u/114918271?s=400&u=4d1009e6c82ee0ae151f7d86a3a8a08b3457f944&v=4" width="100px;" alt="Tristan CALARD" style="border-radius: 50% !important;"/><br /><sub><b>Tristan<br>CALARD</b></sub></a><br />
            </td>
            <td style="width: 20%; text-align: center;">
                <a href="https://github.com/vahand"><img src="https://avatars.githubusercontent.com/u/97789425?v=4" width="100px;" alt="Vahan DUCHER" style="border-radius: 50% !important;"/><br /><sub><b>Vahan<br>Ducher</b></sub></a><br />
            </td>
            <td style="width: 20%; text-align: center;">
                <a href="https://github.com/Matlyce"><img src="https://avatars.githubusercontent.com/u/85689655?v=4" width="100px;" alt="Mathis EMAILLE" style="border-radius: 50% !important;"/><br /><sub><b>Mathis<br>EMAILLE</b></sub></a><br />
            </td>
            <td style="width: 20%; text-align: center;">
                <a href="https://github.com/romainlabed"><img src="https://avatars.githubusercontent.com/u/114907283?v=4" width="100px;" alt="Romain LABED" style="border-radius: 50% !important;"/><br /><sub><b>Romain<br>LABED</b></sub></a><br />
            </td>
            <td style="width: 20%; text-align: center;">
                <a href="https://github.com/Richonn"><img src="https://avatars.githubusercontent.com/u/74425137?v=4" width="100px;" alt="Léandre CACARIE" style="border-radius: 50% !important;"/><br /><sub><b>Léandre<br>CACARIE</b></sub></a><br />
            </td>
        </tr>
    </tbody>
</table>
