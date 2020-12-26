#pragma once
#include <menu/menu.h>
#include <sdk/sdk.h>
#include <sdk/Consts.h>

//void imwrap::const_slider(const char* name, float min, float max, const char* format, int index, const char* tooltip = nullptr) {
//    if (ImGui::Button(utils::format("Reset###re%d", index).c_str())) {
//        consts::values[index] = consts::defs[index];
//        consts::set_float(index, consts::defs[index]);
//    }
//    ImGui::SameLine();
//    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);
//    if (ImGui::SliderFloat(name, &consts::values[index], min, max, format))
//        consts::set_float(index, consts::values[index]);
//    ImGui::PopItemWidth();
//    imwrap::tooltip(tooltip);
//}

void menu::cheats_tab() {
    auto local = sdk::GetGameLogic()->GetLocalPlayer();

    ImGui::Text("This tab is work in progress at the moment.");

    //we don't need to build this for others, since a private feature for now
#if __has_include("code1.txt")
    if (ImGui::Button("Perform magic")) {
        auto pos = local->GetPos();
        local->do_stuff2(int(pos.x / 32.f) - 3, int(pos.y / 32.f) - 3);
    }
#endif

    static int flags = 0;
    ImGui::SliderInt("flags", &flags, 0, 7);
    if (ImGui::Button("Set flags")) { //something that will be added properly later, pretty cool thing
        using fuck = void(__cdecl*)(NetAvatar*, char*);
        static auto func = fuck((uintptr_t)global::gt + 0x3978F0);
        func(local, (char*)&flags);
    }

    if (ImGui::CollapsingHeader("Game constants")) {
        if (ImGui::BeginChild("###Constants", AUTOSIZE(10), true)) {
            int i = 0;
            if (ImGui::Button("Reset all")) {

                 consts::reset_all();

                if (opt::cheat::punch_cooldown_on) //dont reset this if we have the other option on for this
                     consts::set_float(C_PUNCH_RELOAD_TIME, opt::cheat::punch_cooldown_val);
            }
               
            imwrap::const_slider("Max falling speed", 0.f, 2000.f, "%.0f", i++, "Pretty simple");
            imwrap::const_slider("Float power", 0.f, 10000.f, "%.0f", i++, "Higher makes damage flying curve smaller");
            imwrap::const_slider("Wind speed", 0.f, 2000.0f, "%.0f", i++, "The speed winter wind and others push you");
            imwrap::const_slider("Wind speed (against)", 0.f, 200.0f, "%.0f", i++, "The resistance of going against winter wind");
            imwrap::const_slider("Collision max divergence", 0.f, 128.0f, "%.0f", i++, "No clue what this does, no references, no effect");
            imwrap::const_slider("Player drag", -500.f, 2400.0f, "%.0f", i++, "Drag for walking. Low values allow 'moonwalk'.");
            imwrap::const_slider("Initial jump boost", -100.f, -1000.0f, "%.0f", i++, "Affects how high you can jump in a different way from gravity");
            imwrap::const_slider(
                "Control mod (when hurt)", 0.0f, 1.5f, "%0.2f", i++, "Affects how well you are able to control your char when hurt. 1 = normal, 0 = no control.");
            imwrap::const_slider("Lava bounce [x]", -100.f, 1000.0f, "%.0f", i++, "Just how much you bounce from lava/etc horizontally");
            imwrap::const_slider("Lava bounce [y]", -100.f, 1400.0f, "%.0f", i++, "Just how much you bounce from lava/etc vertically");
            imwrap::const_slider("Trampoline bounce", -100.f, 1600.0f, "%.0f", i++, "Bounciness for mushroom/etc");
            imwrap::const_slider("Regular bounce", -100.f, 1600.0f, "%.0f", i++, "Bounciness for pinballs/etc");
            imwrap::const_slider("Default punch knockback", -500.f, 500.0f, "%.0f", i++, "Does not seem to have much effect except for anims (clientside)");
            imwrap::const_slider("Parasol gravity mod", 0.f, 1.0f, "%0.2f", i++, "How much parasol and similar affect gravity");
            imwrap::const_slider("Water gravity mod", 0.f, 2.0f, "%0.2f", i++, "How much water affects gravity");
            imwrap::const_slider("Mars gravity mod", 0.f, 2.0f, "%0.2f", i++, "How much mars affects gravity");
            imwrap::const_slider("Engine speedhack", 0.f, 4000.0f, "%.0f", i++, "Game engine speedhack. Affects various things, some need re-enter to apply fully.");
            imwrap::const_slider("Ghost speed", 0.f, 1000.0f, "%.0f", i++, "When you have the /ghost playmod, not related to ghosts that slime you.");
            imwrap::const_slider(
                "Water levitation time", 0.f, 6000.0f, "%.0f", i++, "Probably the time that the clothes that keep you floating on water have any effect. Didn't test.");
            imwrap::const_slider("Punch cooldown", 0.f, 0.4f, "%0.2f", i++, "Changes punch cooldown, fast punch when small value");
            imwrap::const_slider("Hack report delay (ms)", 100.f, 30000.0f, "%.0f", i++, "For example cheat engine open report, etc. Shouldn't have much of any use");
        }
        ImGui::EndChild();
    }

    if (ImGui::CollapsingHeader("Punch cooldown changer")) {
        static int ix = C_PUNCH_RELOAD_TIME;
        if (ImGui::BeginChild("###cooldownchanger", AUTOSIZE(2), true)) {
            if (ImGui::Checkbox("Enabled###punch", &opt::cheat::punch_cooldown_on)) {
                if (!opt::cheat::punch_cooldown_on)
                    consts::set_float(ix, consts::defs[ix]);
                else
                    consts::set_float(ix, opt::cheat::punch_cooldown_val);
            }
            ImGui::SameLine();
            if (ImGui::SliderFloat("###punchc", &opt::cheat::punch_cooldown_val, 0.0f, 0.4f, "%0.2f") && opt::cheat::punch_cooldown_on) {
                consts::set_float(ix, opt::cheat::punch_cooldown_val);
            }
            ImGui::Text("Does the same thing as the const slider for punch cooldown, just left this as a changer too.");
            ImGui::EndChild();
        }
    }
    if (ImGui::CollapsingHeader("Gravity changer")) {
        if (ImGui::BeginChild("###gravitychanger", AUTOSIZE(2), true)) {
            ImGui::Checkbox("Enabled###grav", &opt::cheat::gravity_on);
            ImGui::SameLine();
            ImGui::SliderFloat("###grav", &opt::cheat::gravity_val, -500.0f, 2000.f, "%0.0f");
            ImGui::Text("Shouldn't ban except negative/zero gravity when in air for too long.");
            ImGui::EndChild();
        }
    }
    if (ImGui::CollapsingHeader("Movement speed changer")) {
        if (ImGui::BeginChild("###movchanger", AUTOSIZE(3), true)) {
            ImGui::Checkbox("Enabled###movtoggle", &opt::cheat::movespeed_on);
            ImGui::SameLine();
            imwrap::checkbox("Instant Stop", opt::cheat::movespeed_stop, "Removes deceleration, resulting in instant stops to movement.");
            ImGui::SameLine();
            imwrap::checkbox("Instant Accel", opt::cheat::movespeed_start, "Removes acceleration, giving you full moving speed instantly.");

            ImGui::SliderFloat("###movspeed", &opt::cheat::movespeed_val, 100.f, 1400.f, "%0.0f");
            ImGui::Text("Shouldn't ban except maybe at really high speeds or special situations.");
            ImGui::EndChild();
        }
    }
    //commented for now because going to be overhauling the changers soon anyways

    //if (ImGui::CollapsingHeader("Acceleration changer")) {
    //    if (ImGui::BeginChild("###accelchanger", AUTOSIZE(2), true)) {
    //        static bool enabled = false;
    //        static float accel = 1200.f;
    //        ImGui::Checkbox("Enabled###accel", &enabled);
    //        ImGui::SameLine();
    //        if (ImGui::SliderFloat("###accel", &accel, 100.f, 4400.f, "%0.0f") && enabled) {
    //            float target = accel;
    //            /*    float temp = local->accel_verify;
    //            local->accel_second = temp + target;
    //            local->accel = temp * 0.5f + target;*/
    //        }
    //        ImGui::EndChild();
    //    }
    //}

    //if (ImGui::CollapsingHeader("Move speed changer 2")) {
    //    if (ImGui::BeginChild("###accelchanger2", AUTOSIZE(2), true)) {
    //        static bool enabled = false;
    //        static float accel = 250.f;
    //        ImGui::Checkbox("Enabled###accel2", &enabled);
    //        ImGui::SameLine();
    //        if (ImGui::SliderFloat("###accel2", &accel, 100.f, 1000.f, "%0.0f") && enabled) {
    //            float target = accel;
    //            /*     float temp = local->speed_verify;
    //            local->speed_second = temp + target;
    //            local->speed = temp * 0.5f + target;*/
    //        }
    //        ImGui::EndChild();
    //    }
    //}
}