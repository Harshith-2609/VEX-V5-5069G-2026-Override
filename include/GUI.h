// // ============================================================
// //  GUI.h — Auton Selector + Debug | ZIPPY 5069G
// //  V5RC Override 2026-27 | LVGL 9.2 | PROS 4
// // ============================================================
// #pragma once
// #include "api.h"

// // Index into the AUTONS[] array defined in Autons.cpp.
// // Defaults to 0, which should always be the safe "do nothing" entry.
// extern volatile int selectedAuton;

// void GUI_runAutonSelector(); // call once in initialize()
// void GUI_showDebugScreen();  // call in opcontrol() or disabled()