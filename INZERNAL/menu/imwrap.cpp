#pragma once
#include "imwrap.h"
#include <core/utils.h>
#include <sdk/Consts.h>


//just ported from old csgo cheat, probably also taken from elsewhere partially
bool imwrap::combo(const char* label, int* currIndex, std::vector<std::string>& values) {
    if (values.empty())
        return false;

    return ImGui::Combo(label, currIndex, vector_getter, static_cast<void*>(&values), values.size());
}

bool imwrap::listbox(const char* label, int* currIndex, std::vector<std::string>& values, int height_in_items) {
    if (values.empty())
        return false;

    return ImGui::ListBox(label, currIndex, vector_getter, static_cast<void*>(&values), values.size(), height_in_items);
}

void imwrap::const_slider(const char* name, float min, float max, const char* format, int index, const char* tooltip) {
    if (ImGui::Button(utils::format("Reset###re%d", index).c_str())) {
        consts::values[index] = consts::defs[index];
        consts::set_float(index, consts::defs[index]);
    }
    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);
    if (ImGui::SliderFloat(name, &consts::values[index], min, max, format))
        consts::set_float(index, consts::values[index]);
    ImGui::PopItemWidth();
    imwrap::tooltip(tooltip);
}

void imwrap::tooltip(const char* tip) {
    if (tip) {
        if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 0.2f) //we do not want to hover when mouse just passes by the item therefore delay
            ImGui::SetTooltip(tip);
    }
}

//taken straight out of imgui_stdlib.cpp, since im using older imgui
bool imwrap::inputstring(const char* label, std::string* str, ImGuiInputTextFlags flags, const char* tip) {
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = nullptr;
    cb_user_data.ChainCallbackUserData = nullptr;
    auto retval = ImGui::InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
    tooltip(tip);
    return retval;
}
