// // ============================================================
// //  GUI.cpp — Auton Selector + Debug | ZIPPY 5069G
// //  V5RC Override 2026-27 | LVGL 9.2 | PROS 4
// //
// //  Do not call pros::lcd::initialize() anywhere in this project.
// //  It conflicts with LVGL and was the root cause of GUI failures
// //  last season.
// // ============================================================

// #include "GUI.h"
// #include "Autons.h"
// #include "main.h"

// // ─────────────────────────────────────────────────────────────
// //  PALETTE — fresh look for Override, distinct from last season
// // ─────────────────────────────────────────────────────────────
// static inline lv_color_t C(uint8_t r, uint8_t g, uint8_t b) {
//     return lv_color_make(r, g, b);
// }

// #define CLR_BG         C(14, 16, 20)
// #define CLR_PANEL      C(22, 25, 31)
// #define CLR_PANEL_HI   C(30, 34, 42)
// #define CLR_ACCENT     C(64, 156, 255)
// #define CLR_ACCENT_DIM C(30, 70, 120)
// #define CLR_TEXT       C(230, 233, 238)
// #define CLR_TEXT_DIM   C(140, 146, 158)
// #define CLR_GREEN      C(72, 199, 142)
// #define CLR_YELLOW     C(230, 184, 64)
// #define CLR_RED        C(224, 90, 90)

// #define SW 480
// #define SH 272
// #define TH 22

// volatile int selectedAuton = 0;

// static lv_obj_t* s_selectorScr = nullptr;
// static lv_obj_t* s_debugScr = nullptr;
// static pros::Task* s_debugTask = nullptr;
// static lv_obj_t* s_btns[16] = {};
// static lv_obj_t* s_rpName = nullptr;

// // ─────────────────────────────────────────────────────────────
// //  HELPERS
// // ─────────────────────────────────────────────────────────────
// static lv_obj_t* mk_rect(lv_obj_t* par, int x, int y, int w, int h, lv_color_t bg, int r = 0) {
//     lv_obj_t* o = lv_obj_create(par);
//     lv_obj_set_size(o, w, h);
//     lv_obj_set_pos(o, x, y);
//     lv_obj_set_style_bg_color(o, bg, 0);
//     lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
//     lv_obj_set_style_border_width(o, 0, 0);
//     lv_obj_set_style_radius(o, r, 0);
//     lv_obj_set_style_pad_all(o, 0, 0);
//     lv_obj_set_scrollbar_mode(o, LV_SCROLLBAR_MODE_OFF);
//     lv_obj_remove_flag(o, LV_OBJ_FLAG_SCROLLABLE);
//     return o;
// }

// static lv_obj_t* mk_label(lv_obj_t* par, const char* txt, int x, int y, lv_color_t col, const lv_font_t* font) {
//     lv_obj_t* l = lv_label_create(par);
//     lv_label_set_text(l, txt);
//     lv_obj_set_pos(l, x, y);
//     lv_obj_set_style_text_color(l, col, 0);
//     lv_obj_set_style_text_font(l, font, 0);
//     lv_obj_set_style_bg_opa(l, LV_OPA_TRANSP, 0);
//     return l;
// }

// // ─────────────────────────────────────────────────────────────
// //  SELECTOR SCREEN — built directly from AUTONS[], nothing
// //  is duplicated here. Add an auton in Autons.cpp and it shows
// //  up on screen automatically.
// // ─────────────────────────────────────────────────────────────
// static void btn_cb(lv_event_t* e) {
//     int idx = (int)(intptr_t)lv_event_get_user_data(e);
//     selectedAuton = idx;
//     for (int i = 0; i < AUTON_COUNT; i++) {
//         bool sel = (i == idx);
//         lv_obj_set_style_bg_color(s_btns[i], sel ? CLR_PANEL_HI : CLR_PANEL, 0);
//         lv_obj_set_style_border_color(s_btns[i], sel ? CLR_ACCENT : CLR_ACCENT_DIM, 0);
//         lv_obj_set_style_border_width(s_btns[i], sel ? 2 : 1, 0);
//     }
//     lv_label_set_text(s_rpName, AUTONS[idx].name);
// }

// static lv_obj_t* build_selector() {
//     lv_obj_t* scr = lv_obj_create(NULL);
//     lv_obj_set_style_bg_color(scr, CLR_BG, 0);
//     lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
//     lv_obj_set_style_pad_all(scr, 0, 0);
//     lv_obj_set_style_border_width(scr, 0, 0);
//     lv_obj_set_scrollbar_mode(scr, LV_SCROLLBAR_MODE_OFF);
//     lv_obj_remove_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

//     mk_rect(scr, 0, 0, SW, TH, CLR_PANEL);
//     mk_rect(scr, 0, TH - 1, SW, 1, CLR_ACCENT);
//     mk_label(scr, "OVERRIDE  5069G  SELECT AUTON", 8, 5, CLR_ACCENT, &lv_font_montserrat_10);

//     const int listX = 10, listY = TH + 8, btnH = 34, gap = 6, listW = 300;
//     const int count = (AUTON_COUNT < 16) ? AUTON_COUNT : 16;

//     for (int i = 0; i < count; i++) {
//         int by = listY + i * (btnH + gap);
//         lv_obj_t* btn = lv_obj_create(scr);
//         lv_obj_set_size(btn, listW, btnH);
//         lv_obj_set_pos(btn, listX, by);
//         bool sel = (i == selectedAuton);
//         lv_obj_set_style_bg_color(btn, sel ? CLR_PANEL_HI : CLR_PANEL, 0);
//         lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
//         lv_obj_set_style_border_color(btn, sel ? CLR_ACCENT : CLR_ACCENT_DIM, 0);
//         lv_obj_set_style_border_width(btn, sel ? 2 : 1, 0);
//         lv_obj_set_style_radius(btn, 6, 0);
//         lv_obj_set_style_pad_all(btn, 0, 0);
//         lv_obj_set_scrollbar_mode(btn, LV_SCROLLBAR_MODE_OFF);
//         lv_obj_remove_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
//         lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);

//         lv_obj_t* lbl = lv_label_create(btn);
//         lv_label_set_text(lbl, AUTONS[i].name);
//         lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
//         lv_obj_set_style_text_color(lbl, CLR_TEXT, 0);
//         lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 10, 0);

//         lv_obj_add_event_cb(btn, btn_cb, LV_EVENT_CLICKED, (void*)(intptr_t)i);
//         s_btns[i] = btn;
//     }

//     int rpX = listX + listW + 16;
//     mk_label(scr, "SELECTED", rpX, listY, CLR_ACCENT, &lv_font_montserrat_10);
//     s_rpName = mk_label(scr, AUTONS[selectedAuton].name, rpX, listY + 18, CLR_TEXT, &lv_font_montserrat_14);
//     mk_label(scr, "5069G  ZIPPY", rpX, SH - 30, CLR_TEXT_DIM, &lv_font_montserrat_10);

//     return scr;
// }

// void GUI_runAutonSelector() {
//     for (int i = 0; i < 16; i++) s_btns[i] = nullptr;
//     s_rpName = nullptr;

//     if (s_selectorScr) {
//         lv_obj_delete(s_selectorScr);
//         s_selectorScr = nullptr;
//     }

//     s_selectorScr = build_selector();
//     lv_screen_load(s_selectorScr);
// }

// // ─────────────────────────────────────────────────────────────
// //  DEBUG SCREEN
// // ─────────────────────────────────────────────────────────────
// static lv_obj_t *s_lX, *s_lY, *s_lH, *s_lAuton, *s_lBat, *s_lConn;
// static lv_obj_t *s_lTDrive, *s_lTFour, *s_lTIntake;

// static lv_obj_t* build_debug() {
//     lv_obj_t* scr = lv_obj_create(NULL);
//     lv_obj_set_style_bg_color(scr, CLR_BG, 0);
//     lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
//     lv_obj_set_style_pad_all(scr, 0, 0);
//     lv_obj_set_style_border_width(scr, 0, 0);
//     lv_obj_set_scrollbar_mode(scr, LV_SCROLLBAR_MODE_OFF);
//     lv_obj_remove_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

//     mk_rect(scr, 0, 0, SW, TH, CLR_PANEL);
//     mk_rect(scr, 0, TH - 1, SW, 1, CLR_ACCENT);
//     mk_label(scr, "OVERRIDE  DEBUG  5069G", 8, 5, CLR_ACCENT, &lv_font_montserrat_10);
//     mk_rect(scr, SW / 2 - 1, TH + 2, 1, SH - TH - 4, CLR_ACCENT_DIM);

//     const int C1 = 10, C2 = SW / 2 + 10, Y0 = TH + 10, LH = 22;

//     mk_label(scr, "ODOMETRY", C1, Y0, CLR_ACCENT, &lv_font_montserrat_10);
//     mk_label(scr, "X", C1, Y0 + 16, CLR_TEXT_DIM, &lv_font_montserrat_10);
//     s_lX = mk_label(scr, "---", C1 + 40, Y0 + 14, CLR_TEXT, &lv_font_montserrat_14);
//     mk_label(scr, "Y", C1, Y0 + 16 + LH, CLR_TEXT_DIM, &lv_font_montserrat_10);
//     s_lY = mk_label(scr, "---", C1 + 40, Y0 + 14 + LH, CLR_TEXT, &lv_font_montserrat_14);
//     mk_label(scr, "HDG", C1, Y0 + 16 + LH * 2, CLR_TEXT_DIM, &lv_font_montserrat_10);
//     s_lH = mk_label(scr, "---", C1 + 40, Y0 + 14 + LH * 2, CLR_TEXT, &lv_font_montserrat_14);

//     mk_label(scr, "AUTON", C1, Y0 + 16 + LH * 3 + 8, CLR_ACCENT, &lv_font_montserrat_10);
//     s_lAuton = mk_label(scr, "---", C1, Y0 + 34 + LH * 3, CLR_TEXT, &lv_font_montserrat_12);

//     mk_label(scr, "CONN", C1, Y0 + 60 + LH * 3, CLR_TEXT_DIM, &lv_font_montserrat_10);
//     s_lConn = mk_label(scr, "---", C1 + 40, Y0 + 58 + LH * 3, CLR_YELLOW, &lv_font_montserrat_12);

//     mk_label(scr, "BATTERY", C2, Y0, CLR_ACCENT, &lv_font_montserrat_10);
//     s_lBat = mk_label(scr, "---%", C2 + 60, Y0 - 2, CLR_GREEN, &lv_font_montserrat_14);

//     mk_label(scr, "TEMPS", C2, Y0 + 30, CLR_ACCENT, &lv_font_montserrat_10);
//     mk_label(scr, "DRIVE", C2, Y0 + 46, CLR_TEXT_DIM, &lv_font_montserrat_10);
//     s_lTDrive = mk_label(scr, "--C", C2 + 60, Y0 + 44, CLR_GREEN, &lv_font_montserrat_12);
//     mk_label(scr, "4-BAR", C2, Y0 + 46 + LH, CLR_TEXT_DIM, &lv_font_montserrat_10);
//     s_lTFour = mk_label(scr, "--C", C2 + 60, Y0 + 44 + LH, CLR_GREEN, &lv_font_montserrat_12);
//     mk_label(scr, "INTAKE", C2, Y0 + 46 + LH * 2, CLR_TEXT_DIM, &lv_font_montserrat_10);
//     s_lTIntake = mk_label(scr, "--C", C2 + 60, Y0 + 44 + LH * 2, CLR_GREEN, &lv_font_montserrat_12);

//     return scr;
// }

// static lv_color_t tempClr(double t) {
//     if (t < 45.0) return CLR_GREEN;
//     if (t < 58.0) return CLR_YELLOW;
//     return CLR_RED;
// }

// static void debugTask(void*) {
//     char buf[32];
//     while (true) {
//         auto pose = chassis.getPose();
//         snprintf(buf, sizeof(buf), "%.1f", pose.x);
//         lv_label_set_text(s_lX, buf);
//         snprintf(buf, sizeof(buf), "%.1f", pose.y);
//         lv_label_set_text(s_lY, buf);
//         snprintf(buf, sizeof(buf), "%.1f", pose.theta);
//         lv_label_set_text(s_lH, buf);

//         lv_label_set_text(s_lAuton, AUTONS[selectedAuton].name);

//         bool conn = pros::competition::is_connected();
//         lv_label_set_text(s_lConn, conn ? "FIELD" : "CTRLR");
//         lv_obj_set_style_text_color(s_lConn, conn ? CLR_GREEN : CLR_YELLOW, 0);

//         int bat = (int)pros::battery::get_capacity();
//         snprintf(buf, sizeof(buf), "%d%%", bat);
//         lv_label_set_text(s_lBat, buf);
//         lv_obj_set_style_text_color(s_lBat, bat > 60 ? CLR_GREEN : (bat > 30 ? CLR_YELLOW : CLR_RED), 0);

//         double tDrive = (DriveL.get_temperature() + DriveR.get_temperature()) / 2.0;
//         double tFour = FourBar.get_temperature();
//         double tIntake = intake.get_temperature();

//         snprintf(buf, sizeof(buf), "%.0fC", tDrive);
//         lv_label_set_text(s_lTDrive, buf);
//         lv_obj_set_style_text_color(s_lTDrive, tempClr(tDrive), 0);

//         snprintf(buf, sizeof(buf), "%.0fC", tFour);
//         lv_label_set_text(s_lTFour, buf);
//         lv_obj_set_style_text_color(s_lTFour, tempClr(tFour), 0);

//         snprintf(buf, sizeof(buf), "%.0fC", tIntake);
//         lv_label_set_text(s_lTIntake, buf);
//         lv_obj_set_style_text_color(s_lTIntake, tempClr(tIntake), 0);

//         pros::delay(250);
//     }
// }

// void GUI_showDebugScreen() {
//     if (!s_debugScr) s_debugScr = build_debug();
//     lv_screen_load(s_debugScr);
//     if (!s_debugTask) s_debugTask = new pros::Task(debugTask);
// }