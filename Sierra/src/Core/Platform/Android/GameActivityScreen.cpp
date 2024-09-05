//
// Created by Nikolay Kanchevski on 12/14/23.
//

#include "GameActivityScreen.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    GameActivityScreen::GameActivityScreen(const ConfigurationScreenCreateInfo& createInfo)
        : configuration(createInfo.configuration)
    {
        // Attach thread to JNI
        JNIEnv* env = createInfo.gameActivity->env;
        createInfo.gameActivity->vm->AttachCurrentThread(&env, nullptr);
        jclass activityClass = env->GetObjectClass(createInfo.gameActivity->javaGameActivity);

        // Get window manager
        jmethodID getWindowManager = env->GetMethodID(activityClass, "getWindowManager", "()Landroid/view/WindowManager;");
        jobject windowManager = env->CallObjectMethod(createInfo.gameActivity->javaGameActivity, getWindowManager);
        jclass windowManagerClass = env->GetObjectClass(windowManager);

        // Get window metrics
        jmethodID getCurrentWindowMetrics = env->GetMethodID(windowManagerClass, "getCurrentWindowMetrics", "()Landroid/view/WindowMetrics;");
        jobject windowMetrics = env->CallObjectMethod(windowManager, getCurrentWindowMetrics);
        jclass windowMetricsClass = env->GetObjectClass(windowMetrics);

        // Get bounds
        jmethodID getBounds = env->GetMethodID(windowMetricsClass, "getBounds", "()Landroid/graphics/Rect;");
        jobject bounds = env->CallObjectMethod(windowMetrics, getBounds);
        jclass rectClass = env->GetObjectClass(bounds);

        // Read bounds
        const int32 boundsLeft = env->GetIntField(bounds, env->GetFieldID(rectClass, "left", "I"));
        const int32 boundsTop = env->GetIntField(bounds, env->GetFieldID(rectClass, "top", "I"));
        const int32 boundsRight = env->GetIntField(bounds, env->GetFieldID(rectClass, "right", "I"));
        const int32 boundsBottom = env->GetIntField(bounds, env->GetFieldID(rectClass, "bottom", "I"));

        // Get window insets
        jmethodID getWindowInsets = env->GetMethodID(windowMetricsClass, "getWindowInsets", "()Landroid/view/WindowInsets;");
        jobject windowInsets = env->CallObjectMethod(windowMetrics, getWindowInsets);
        jclass windowInsetsClass = env->GetObjectClass(windowInsets);

        // Get insets (STATUS_BARS | NAVIGATION_BARS)
        jmethodID getInsets = env->GetMethodID(windowInsetsClass, "getInsets", "(I)Landroid/graphics/Insets;");
        jobject insets = env->CallObjectMethod(windowInsets, getInsets, 0b0011);
        jclass insetsClass = env->GetObjectClass(insets);

        // Read insets
        const int32 insetsLeft = env->GetIntField(insets, env->GetFieldID(insetsClass, "left", "I"));
        const int32 insetsTop = env->GetIntField(insets, env->GetFieldID(insetsClass, "top", "I"));
        const int32 insetsRight = env->GetIntField(insets, env->GetFieldID(insetsClass, "right", "I"));
        const int32 insetsBottom = env->GetIntField(insets, env->GetFieldID(insetsClass, "bottom", "I"));

        // Get model
        jclass buildClass = env->FindClass("android/os/Build");
        jfieldID model = env->GetStaticFieldID(buildClass, "MODEL", "Ljava/lang/String;");
        jstring modelName = reinterpret_cast<jstring>(env->GetStaticObjectField(buildClass, model));
        name = std::string(env->GetStringUTFChars(modelName, nullptr));

        // Get display
        jmethodID getDisplay = env->GetMethodID(activityClass, "getDisplay", "()Landroid/view/Display;");
        jobject display = env->CallObjectMethod(createInfo.gameActivity->javaGameActivity, getDisplay);
        jclass displayClass = env->GetObjectClass(display);

        // Get refresh rate
        jmethodID getRefreshRate = env->GetMethodID(displayClass, "getRefreshRate", "()F");
        refreshRate = static_cast<uint32>(env->CallFloatMethod(display, getRefreshRate));

        // Detach thread
        createInfo.gameActivity->vm->DetachCurrentThread();

        // Calculate origin and size
        origin = { 0, 0 };
        size = { boundsRight - boundsLeft, boundsBottom - boundsTop };

        // Add up extents to get work area origin
        workAreaOrigin = origin;
        workAreaOrigin.x += insetsLeft;
        workAreaOrigin.y += insetsBottom;

        // Remove extents to get work area size
        workAreaSize = size;
        workAreaSize.x -= insetsLeft + insetsRight;
        workAreaSize.y -= insetsBottom + insetsTop;
    }

    /* --- GETTER METHODS --- */

    ScreenOrientation GameActivityScreen::GetOrientation() const
    {
        const int32 orientation = AConfiguration_getOrientation(configuration);
        if (orientation == ACONFIGURATION_ORIENTATION_PORT) return ScreenOrientation::Portrait;
        return ScreenOrientation::Landscape;
    }

}