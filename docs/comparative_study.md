# Comparative Study

This page presents a detailed comparative study of various methodologies, tools, and approaches relevant to the project.

## Graphical: Raylib

Raylib is a simple and easy-to-use C library designed for developing games and multimedia applications. It is a lightweight, cross-platform framework that supports various features like 2D and 3D rendering, sound, input management, and more. Raylib stands out for its straightforward approach, making it a popular choice for both beginners and experienced developers working on projects that require fast, efficient rendering and multimedia handling.

### Key Advantages:
- Lightweight and easy to integrate into any project.
- Simple and clear API for quick learning.
- Supports 2D and 3D graphics, as well as audio and input management.
- Cross-platform compatibility (Windows, Linux, macOS, and more).
- Active community and a variety of resources available for learning.

### Key Disadvantages:
- Limited high-level abstraction compared to other game engines.
- May require more manual control for complex projects.
- Smaller ecosystem and fewer third-party tools than larger engines like Unity or Unreal Engine.

## Architecture: ECS (Entity-Component-System)

ECS is an architectural pattern used in game development and simulations. It organizes data into entities, components, and systems, helping developers achieve more flexibility and performance. Each entity is a unique object, and its components define its attributes (like position, velocity, or health). Systems are responsible for processing entities that have certain components.

### Key Advantages:
- Improves performance through data-oriented design, as systems can operate on large sets of entities efficiently.
- Highly scalable, making it ideal for large and complex games or simulations.
- Promotes clean separation of concerns, with logic organized into reusable systems.
- Facilitates parallel processing and multithreading, maximizing performance.

### Key Disadvantages:
- More complex to learn and implement compared to traditional object-oriented design.
- Requires careful planning and organization to avoid becoming overly complicated.
- Not always the best fit for smaller projects with simpler requirements.

## Protocol: Binary UDP

Binary UDP (User Datagram Protocol) is a communication protocol that is lightweight, fast, and used for transmitting data in a format that is machine-readable. UDP is connectionless, meaning it does not establish a reliable connection before sending data, which makes it faster but less reliable than other protocols like TCP.

### Key Advantages:
- Fast transmission with minimal overhead.
- Ideal for real-time applications such as multiplayer games or streaming, where speed is crucial.
- Low latency due to its connectionless nature.

### Key Disadvantages:
- No guarantee of message delivery, which can lead to data loss.
- Does not have built-in error correction or retransmission features.
- More difficult to implement error handling and reliability features manually.

## License: GNU General Public License v3.0

The GNU General Public License v3.0 (GPL-3.0) is a free software license that ensures the software remains open source and allows users to freely modify, distribute, and use the software. It aims to protect the rights of users and developers by preventing proprietary use of open-source software.

### Key Advantages:
- Ensures software remains free and open, protecting users' freedom to modify and distribute it.
- Provides strong copyleft protection, meaning derivative works must also be licensed under the GPL.
- Wide acceptance in the open-source community, making it ideal for projects that want to contribute to or benefit from the open-source ecosystem.

### Key Disadvantages:
- May discourage commercial use or proprietary licensing due to the copyleft requirements.
- Requires careful compliance with licensing terms, especially when redistributing or modifying the software.
- Some developers or companies might prefer more permissive licenses like MIT or Apache for less restrictive terms.
