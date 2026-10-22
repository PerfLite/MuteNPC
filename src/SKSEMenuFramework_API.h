#pragma once
#include <Windows.h>

namespace SKSEMenuFramework
{
    using RenderCallback = void (*)();
    using AddSectionItem_t = void (*)(const char*, RenderCallback);
    using AddWindow_t = void (*)(RenderCallback);
    using RegisterHudElement_t = void (*)(RenderCallback);

    struct ImVec2
    {
        float x{ 0.0f };
        float y{ 0.0f };
    };

    using igText_t = void (*)(const char*, ...);
    using igCheckbox_t = bool (*)(const char*, bool*);
    using igSliderFloat_t = bool (*)(const char*, float*, float, float, const char*, int);
    using igSliderInt_t = bool (*)(const char*, int*, int, int, const char*, int);
    using igCombo_Str_t = bool (*)(const char*, int*, const char*, int);
    using igButton_t = bool (*)(const char*, ImVec2);
    using igSeparator_t = void (*)();
    using igSameLine_t = void (*)(float, float);
    using igInputText_t = bool (*)(const char*, char*, size_t, int, void*, void*);
    using igSetNextItemWidth_t = void (*)(float);
    using igCollapsingHeader_t = bool (*)(const char*, int);
    using IsAnyBlockingWindowOpened_t = bool (*)();
    using igArrowButton_t = bool (*)(const char*, int);
    using igIsItemHovered_t = bool (*)(int);
    using igIsItemActive_t = bool (*)();
    using igPushButtonRepeat_t = void (*)(bool);
    using igPopButtonRepeat_t = void (*)();

    inline AddSectionItem_t AddSectionItem = nullptr;
    inline AddWindow_t AddWindow = nullptr;
    inline RegisterHudElement_t RegisterHudElement = nullptr;
    inline igText_t igText = nullptr;
    inline igCheckbox_t igCheckbox = nullptr;
    inline igSliderFloat_t igSliderFloat = nullptr;
    inline igSliderInt_t igSliderInt = nullptr;
    inline igCombo_Str_t igCombo_Str = nullptr;
    inline igButton_t igButton = nullptr;
    inline igSeparator_t igSeparator = nullptr;
    inline igSameLine_t igSameLine = nullptr;
    inline igInputText_t igInputText = nullptr;
    inline igSetNextItemWidth_t igSetNextItemWidth = nullptr;
    inline igCollapsingHeader_t igCollapsingHeader = nullptr;
    inline IsAnyBlockingWindowOpened_t IsAnyBlockingWindowOpened = nullptr;
    inline igArrowButton_t igArrowButton = nullptr;
    inline igIsItemHovered_t igIsItemHovered = nullptr;
    inline igIsItemActive_t igIsItemActive = nullptr;
    inline igPushButtonRepeat_t igPushButtonRepeat = nullptr;
    inline igPopButtonRepeat_t igPopButtonRepeat = nullptr;

    inline bool Init()
    {
        auto hMod = GetModuleHandleA("SKSEMenuFramework.dll");
        if (!hMod) {
            hMod = LoadLibraryA("SKSEMenuFramework.dll");
        }
        if (!hMod) {
            return false;
        }

        AddSectionItem = reinterpret_cast<AddSectionItem_t>(GetProcAddress(hMod, "AddSectionItem"));
        AddWindow = reinterpret_cast<AddWindow_t>(GetProcAddress(hMod, "AddWindow"));
        RegisterHudElement = reinterpret_cast<RegisterHudElement_t>(GetProcAddress(hMod, "RegisterHudElement"));
        igText = reinterpret_cast<igText_t>(GetProcAddress(hMod, "igText"));
        igCheckbox = reinterpret_cast<igCheckbox_t>(GetProcAddress(hMod, "igCheckbox"));
        igSliderFloat = reinterpret_cast<igSliderFloat_t>(GetProcAddress(hMod, "igSliderFloat"));
        igSliderInt = reinterpret_cast<igSliderInt_t>(GetProcAddress(hMod, "igSliderInt"));
        igCombo_Str = reinterpret_cast<igCombo_Str_t>(GetProcAddress(hMod, "igCombo_Str"));
        igButton = reinterpret_cast<igButton_t>(GetProcAddress(hMod, "igButton"));
        igSeparator = reinterpret_cast<igSeparator_t>(GetProcAddress(hMod, "igSeparator"));
        igSameLine = reinterpret_cast<igSameLine_t>(GetProcAddress(hMod, "igSameLine"));
        igInputText = reinterpret_cast<igInputText_t>(GetProcAddress(hMod, "igInputText"));
        igSetNextItemWidth = reinterpret_cast<igSetNextItemWidth_t>(GetProcAddress(hMod, "igSetNextItemWidth"));
        igCollapsingHeader = reinterpret_cast<igCollapsingHeader_t>(GetProcAddress(hMod, "igCollapsingHeader_TreeNodeFlags"));
        IsAnyBlockingWindowOpened = reinterpret_cast<IsAnyBlockingWindowOpened_t>(GetProcAddress(hMod, "IsAnyBlockingWindowOpened"));
        igArrowButton = reinterpret_cast<igArrowButton_t>(GetProcAddress(hMod, "igArrowButton"));
        igIsItemHovered = reinterpret_cast<igIsItemHovered_t>(GetProcAddress(hMod, "igIsItemHovered"));
        igIsItemActive = reinterpret_cast<igIsItemActive_t>(GetProcAddress(hMod, "igIsItemActive"));
        igPushButtonRepeat = reinterpret_cast<igPushButtonRepeat_t>(GetProcAddress(hMod, "igPushButtonRepeat"));
        igPopButtonRepeat = reinterpret_cast<igPopButtonRepeat_t>(GetProcAddress(hMod, "igPopButtonRepeat"));

        return AddSectionItem != nullptr;
    }
}
