# Naval Radar Simulator

A C-based naval vessel simulation system that models different types of boats with unique behaviors on a maritime radar grid.

## Author
**Gonçalo Gonçalves**   
Programming Languages I - 2024/2025

## Overview

This project implements a naval radar simulator that tracks and simulates the movement of different types of vessels on a maritime grid. The simulator supports real-time vessel tracking, collision detection, historical state management, and specialized boat behaviors.

## Features

### Vessel Types Implemented (for last digit 1)
1. **ProfPaiMau (Type 1)** - Invincible vessel that doesn't sink during collisions
2. **Cruiser (Type 2)** - Doubles speed when no nearby vessels are detected
3. **Submarine (Type 3)** - Alternates visibility every 5 frames (appears/disappears)
4. **Fishing Boat (Type 4)** - Stops for 5 frames out of every 10 frames

### Core Functionality
- ✅ **Real-time simulation** with frame-by-frame progression
- ✅ **Collision detection** and vessel interaction
- ✅ **Historical state management** with reverse tracking
- ✅ **Dynamic vessel insertion/modification** during simulation
- ✅ **Collision prediction** system
- ✅ **Speed and movement analytics** per vessel
- ✅ **File I/O** for loading initial states and saving simulation results

## Technical Architecture

### Data Structures
- **`NoVessel`** - Static vessel information (name, type, initial parameters)
- **`EntidadeIED`** - Dynamic vessel state (position, velocity, linked list node)
- **`BaseDados`** - Frame container holding all visible vessels and simulation state

### Key Algorithms
- **Velocity Calculation**: Converts angular direction to X,Y velocity components
- **Collision Detection**: Spatial overlap detection with type-specific behavior
- **Behavior System**: Frame-based state machines for each vessel type
- **Memory Management**: Dynamic allocation with proper cleanup

## Usage

### Compilation
```bash
gcc -o naval_simulator main.c -lm
```

### Execution
```bash
./naval_simulator <input_file> <grid_size> <initial_frames> <output_file>
```

**Parameters:**
- `input_file`: Initial vessel configuration file
- `grid_size`: Grid dimensions in format `WIDTHxHEIGHT` (e.g., `50x50`)
- `initial_frames`: Number of frames to simulate initially
- `output_file`: Output file for final simulation state

### Example
```bash
./naval_simulator vessels.txt 100x80 10 simulation_result.txt
```

## Input File Format

Each line represents a vessel with the following format:
```
<NAME> <X> <Y> <ANGLE> <SPEED> <TYPE>
```

**Example:**
```
A 10 15 45 2 1
B 20 25 180 3 2
C 5 30 90 1 3
```

## Interactive Menu

The simulator provides an interactive menu with the following options:

1. **Update Simulation** - Advance simulation by specified frames
2. **Insert/Modify Vessel** - Add new vessels or modify existing ones
3. **Collision Prediction** - Forecast potential vessel collisions
4. **Reverse History Tracking** - Navigate to previous simulation states
5. **Vessel Speed Analytics** - Calculate average speed and movement statistics

## Vessel Behaviors

### ProfPaiMau (Type 1)
- **Special Ability**: Survives all collisions
- **Movement**: Standard velocity-based movement
- **Use Case**: Indestructible patrol vessel

### Cruiser (Type 2)
- **Special Ability**: Doubles speed when no vessels within 4-unit radius
- **Movement**: Dynamic speed adjustment based on proximity
- **Use Case**: Fast reconnaissance vessel

### Submarine (Type 3)
- **Special Ability**: Cycles visibility (visible 5 frames, invisible 5 frames)
- **Movement**: Standard movement when visible, undetectable when submerged
- **Use Case**: Stealth operations

### Fishing Boat (Type 4)
- **Special Ability**: Alternates between moving (5 frames) and stationary (5 frames)
- **Movement**: Stop-and-go pattern simulating fishing activities
- **Use Case**: Commercial fishing vessel

## Technical Specifications

- **Language**: C (C99 standard)
- **Dependencies**: Standard C library, math library (`-lm`)
- **Memory Management**: Dynamic allocation with comprehensive cleanup
- **Grid System**: Cartesian coordinate system with boundary checking
- **Collision System**: Position-based collision detection
- **Frame Rate**: 1 frame = 1 second of simulation time

## File Structure

```
├── main.c                 # Main source code
├── README.md             # This documentation
├── vessels.txt           # Sample input file
└── simulation_result.txt # Output file
```

## Error Handling

The simulator includes comprehensive error handling for:
- Invalid input file formats
- Out-of-bounds vessel positions
- Memory allocation failures
- Invalid vessel types or parameters
- File I/O errors

## Future Enhancements

- [ ] Graphical visualization
- [ ] Network multiplayer support
- [ ] Advanced pathfinding algorithms
- [ ] Weather system effects
- [ ] Fuel consumption modeling

## License

This project is developed for educational purposes as part of the Programming Languages I course.

---

**Development Environment**: CLion IDE  
**Version Control**: Git/GitHub  
**Testing**: Manual testing with various vessel configurations
