#pragma once
#include <menu/menu.h>
#include <sdk/sdk.h>

void menu::enhancements_tab() {
    
    if (ImGui::CollapsingHeader("Custom Server")) {
        if (ImGui::BeginChild("###customserver", AUTOSIZE(2.f), true)) {
            imwrap::checkbox("Enable###custom", opt::custom_server_on, "You can use this to connect to private servers too");
            ImGui::SameLine();
            imwrap::inputstring("##serverval", &opt::custom_server_val, 0);
            ImGui::Text("Works with growtopia2 (gt1 is usually broken), and private servers.");
            ImGui::EndChild();
        }
    }
    if (ImGui::CollapsingHeader("Login info spoofing")) {
        if (ImGui::BeginChild("###loginspoof", ImVec2(ImGui::GetWindowWidth() * 0.93f, 60.f), true)) {
            ImGui::Checkbox("Enable###spoof", &opt::spoof_login);
            ImGui::SameLine();
            imwrap::checkbox("spoof name", opt::spoof_name, "sets requestedName to randomized string");

            //  menu::InputTextSTL("##serverval", &opt::custom_server_val);
            ImGui::EndChild();
        }
    }

    ImGui::Columns(3);
    imwrap::checkbox("See ghosts", opt::see_ghosts, "Allows you to see ghosts as the name says");

    ImGui::NextColumn();

    imwrap::checkbox("Mod zoom", opt::mod_zoom, "Allows you to zoom as far out as you want to, like mods");
    
  //TODO: clean up enhancements and cheats, sort them out and add missing options

    ImGui::NextColumn();

    imwrap::checkbox("TP on click", opt::tp_click, "Teleports to cursor position when you press ctrl + left click");

    ImGui::EndColumns();

}