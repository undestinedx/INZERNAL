#pragma once
#include <menu/menu.h>
#include <sdk/sdk.h>

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

    printf("%f\n", * (float*)((uintptr_t)global::app + 3604));
    static float speed = 400.0f;
    if (ImGui::SliderFloat("Max fall speed", &speed, 0.f, 3000.0f)) {  
        EncryptedFloat* ptr = (EncryptedFloat*)((uintptr_t)global::gt + 0x6854A8);
        float total = 0.0f;
        float delta = speed - ptr->get();
        ptr->set(speed);
        *(float*)((uintptr_t)global::app + 3604) += delta;
    }
    static float control = 0.15f;
    if (ImGui::SliderFloat("Control mod when hurt", &control, 0.f, 2.f)) {
        EncryptedFloat* ptr = (EncryptedFloat*)((uintptr_t)global::gt + 0x685518);
        float total = 0.0f;
        float delta = control - ptr->get();
        ptr->set(control);
        *(float*)((uintptr_t)global::app + 3604) += delta;
    }
    static float bounce = 300.0f;
    if (ImGui::SliderFloat("Lava bounce X", &bounce, 0.f, 1000.0f)) {
        EncryptedFloat* ptr = (EncryptedFloat*)((uintptr_t)global::gt + 0x685528);
        float total = 0.0f;
        float delta = bounce - ptr->get();
        ptr->set(bounce);
        *(float*)((uintptr_t)global::app + 3604) += delta;
    }
    static float thousand = 1000.0f;
    if (ImGui::SliderFloat("Physics speed", &thousand, 0.f, 3000.0f)) {
        EncryptedFloat* ptr = (EncryptedFloat*)((uintptr_t)global::gt + 0x6855A8);
        float total = 0.0f;
        float delta = thousand - ptr->get();
        ptr->set(thousand);
      //  *(float*)((uintptr_t)global::app + 3604) += delta;
    }
    static float watertime = 3000.0f;
    if (ImGui::SliderFloat("Water levitation time", &watertime, 0.f, 5000.f)) {
        EncryptedFloat* ptr = (EncryptedFloat*)((uintptr_t)global::gt + 0x6855C8);
        float total = 0.0f;
        float delta = watertime - ptr->get();
        ptr->set(watertime);
        //  *(float*)((uintptr_t)global::app + 3604) += delta;
    }
    static float punch = 0.2f;
    if (ImGui::SliderFloat("Punch reload time", &punch, 0.f, 2.f)) {
        EncryptedFloat* ptr = (EncryptedFloat*)((uintptr_t)global::gt + 0x6855D8);
        float total = 0.0f;
        float delta = punch - ptr->get();
        ptr->set(punch);
        //  *(float*)((uintptr_t)global::app + 3604) += delta;
    }
    if (ImGui::CollapsingHeader("Punch/build cooldown changer")) {
        if (ImGui::BeginChild("###cooldownchanger", ImVec2(ImGui::GetWindowWidth() * 0.93f, 60.f), true)) {
            ImGui::Checkbox("Enabled###punch", &opt::cheat::punch_cooldown_on);
            ImGui::SameLine();
            ImGui::SliderFloat("####pcoold", &opt::cheat::punch_cooldown_val, 0.05f, 0.4f, "%0.2f");
            ImGui::Text("Shouldn't ban but low values will disconnect.");
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