# CNC Arm — STM32-Based 2-Axis Robotic Manipulator

A two-axis CNC robotic arm developed as my Bachelor's thesis project in Electrical Engineering.

The system is based on an **STM32F103C8T6 (Blue Pill)** microcontroller and uses a planar two-link mechanism to perform precise movements and draw images on a flat surface.

## Key Features

- **Inverse kinematics:** Converts Cartesian coordinates (X, Y) into joint angles for a two-link robotic arm.
- **Synchronized stepper control:** Coordinates both motors with a parabolic acceleration and deceleration profile.
- **BMP image processing:** Reads bitmap images from an SD card and converts pixel data into drawing commands.
- **Microstepping:** Uses TB6600 drivers for precise motor positioning.
- **Peripheral control:** SPI communication, PWM servo control, and limit switches for positioning and safety.

## Hardware

| Component | Description |
|---|---|
| Microcontroller | STM32F103C8T6 (Blue Pill) |
| Main motor | NEMA 23 with 3:1 reduction |
| Secondary motor | NEMA 17 |
| Motor drivers | 2 × TB6600 |
| End effector | Servo-actuated drawing tool |
| Storage | SD card (SPI interface) |
| Power supply | 36 V, 250 W |
| Voltage regulation | LM2596-based 36 V to 5 V buck converter |

The manipulator uses two 20 cm arms, providing a theoretical maximum reach of 40 cm.

## Software

The firmware is written in **C**, using the **STM32 HAL library** and developed in **STM32CubeIDE**.

The main software components handle:

1. Reading and processing BMP files from an SD card.
2. Calculating inverse kinematics for target coordinates.
3. Converting angular displacements into motor steps.
4. Generating synchronized step pulses with smooth acceleration and deceleration.
5. Controlling the drawing tool using PWM.

## Development

This project includes both hardware and software development, from component selection and circuit design to firmware implementation and assembly of a working prototype.

**Tools:** STM32CubeIDE, STM32CubeMX, C, STM32 HAL, Multisim.

## Hardware
<img width="958" height="935" alt="image" src="https://github.com/user-attachments/assets/c9aa952f-9195-48e9-ad9d-02328666d22e" />
<img width="1280" height="960" alt="image" src="https://github.com/user-attachments/assets/794064ec-24ac-4555-abc1-ef1ab05ce62c" />

## Result example
<img width="599" height="456" alt="image" src="https://github.com/user-attachments/assets/bf2adbca-b641-477e-9c19-65f2b0b5dc00" />

*Developed as a Bachelor's thesis project at Saint Petersburg Mining University, 2026.*
