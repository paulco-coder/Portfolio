# KSP Mission Planning: Mun Robotic Mission

This folder contains calculations, planning, and save files related to a robotic space mission to the Mun in *Kerbal Space Program (KSP)*.
*Challenge Reference:* [KSC - Kerbal Space Challenge](https://www.kerbalspacechallenge.fr/fr/ksc-fr/)

## 🎯 Mission Objective
Starting with a complete spacecraft (command module + lander) on a lunar (Mun) flyby trajectory, the goal is to perform the following steps with high precision and efficiency:
1. **Capture and orbit insertion** around the Mun.
2. **Precision landing** of the lander within 5 km of the target zone `{26°40’44”N 33°5’31”E}`.
3. **Surface stay** of at least 30 minutes for scientific experiments.
4. **Ascent and orbital Rendezvous (RDV)** with the command module left in lunar orbit.
5. **Return trip to Kerbin** with a highly targeted splashdown / landing within 60 km of the recovery coordinates `{4°N ; 68°30’W}` via a single atmospheric entry window.

### ☀️ Mun Landing Constraints
The mission requires meticulous planning regarding lighting to ensure safety:
- Landing in daylight conditions.
- **Solar elevation** close to 30° for good perception of the terrain.
- **Relative Solar Azimuth (SRA)** < 90°: landing with the sun behind to prevent cameras/pilots from being dazzled.
- Minimize Delta-V ($\Delta V$) cost, particularly by employing the *Suicide Burn* (last-minute deceleration) method.

## 📁 Folder Content

- 📓 **`Kerbin_Mun_precise_landings.ipynb`**: The reference interactive notebook detailing each step with orbital mathematics, Python code, and explanatory diagrams:
  - **Phase 1:** Capture Calculation (Circularization).
  - **Phase 2:** Calculation of optimal landing windows (elevation and azimuth graphs) and descent.
  - **Phase 3:** Time prediction for the exact moment of surface takeoff to optimize the orbital rendezvous.
  - **Phase 4:** Mathematical synchronization of the return to Kerbin to coincide with the planet's rotation and target the correct arrival longitude.
- 📓 **`Kerbin_Mun_highly_precise_claculations.ipynb`**: An additional notebook dedicated to mathematical calculations and extremely high-precision maneuvers.
- 🖼️ **`images/`**: The collection of explanatory diagrams, celestial mechanics graphs, and screenshots serving as visual aids in the Notebook.
- 💾 **`Saves_Mission_Mun_precise_landings/`**: The simulation save files `.sfs` and `.loadmeta` reflecting the step-by-step progress of the mission in-game (from the first correction maneuver, powered descent, and docking to the final landing on Kerbin).
- 📑 **`MissionPoster.pdf`**: Presentation poster of the mission.

