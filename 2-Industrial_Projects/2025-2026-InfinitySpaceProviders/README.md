# Autonomous Space Drone Navigation Module - InfinitySpaceProviders (2025-2026)

**Team:** Paul Collette, Charles Roger, Benjamin Lotito, Alexandre François

**Institution:** ESTACA - in collaboration with  InfinitySpaceProviders

**Project type:** Group Engineering Research Project (P2I - Integrated Engineering Project)

**Supervisors:** Michaêl VERT

## Project Objectives

This project aims to design and validate a functional prototype both physical and virtual of an autonomous navigation module for embedded space drones. These unmanned systems will be deployed to perform inspection and servicing missions in orbit.

The navigation module must enable drones to:

- Precisely determine their position in space
- Navigate autonomously without external control
- Identify and interact with target objects
- Maintain communication with other drones throughout the mission
- Integrate perception, data processing, and communication functions
- Operate reliably in potentially unstable space environments

---

## Technical Approach

### Perception and Environmental Understanding

- **Multi-sensor Integration:** Integration of camera, Lidar, and Inertial Measurement Unit (IMU) sensors
- **Target Detection and Tracking:** Recognition algorithms and 3D reconstruction for object identification
- **Real-time Processing:** On-board data processing for immediate decision-making

### On-Board Control and Autonomy

- **Attitude and Orbital Control:** Autonomous control loops using PID or LQR control strategies
- **Visual Stability:** Continuous real-time tracking to maintain visual lock on target objects
- **Autonomous Decision Making:** Self-contained logic for mission execution without ground intervention

### Multi-Drone Cooperation

- **Inter-drone Communication Architecture:** Distributed network topology for drone-to-drone and drone-to-base communication
- **Information Sharing:** Exchange of location data and mission status to coordinate movements
- **Distributed Organization:** Lightweight swarm-based architecture for scalability and robustness

### Simulation and Validation

- **Virtual Environment:** 3D simulation models for orbital rendezvous scenarios
- **Physical Testbed:** Indoor mobile mockup or planar test facility for prototype validation
- **End-to-End Verification:** Complete mission simulation from launch through rendezvous to separation

---

## Project Timeline and Deliverables

## Target Competencies

- **Embedded Computer Vision:** Real-time image processing and target recognition
- **Autonomous Systems and Embedded AI:** Intelligent decision-making algorithms and adaptive control
- **Automatic Control and Navigation:** Guidance, navigation, and control theory
- **Robotic Software Architecture:** Development frameworks including Simulink, Python, C++, and related platforms

---

## Confidentiality Notice

The content of this project is governed by a non-disclosure agreement.