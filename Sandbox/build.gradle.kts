plugins {
    id("com.android.application")
    id("kotlin-android")
}

android {
    namespace = "com.sierra.Sandbox"

    compileSdk = 34
    ndkVersion = "25.1.8937393"

    sourceSets.get("main").res.srcDirs(".android/res/")
    sourceSets.get("main").kotlin.srcDirs(".android/kotlin/")
    sourceSets.get("main").manifest.srcFile(".android/AndroidManifest.xml")
    
    defaultConfig {
        applicationId = "com.sierra.Sandbox"
        minSdk = 30
        targetSdk = 34
        versionName = "1.0.0"
        buildDir = file("bin/Android/Sandbox/")

        shaders {
            glslcArgs += "-c"
        }

        externalNativeBuild {
            cmake {
                arguments += "-DANDROID_STL=c++_shared"
                cppFlags += "-std=c++20"
            }
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = true
            proguardFiles(getDefaultProguardFile("proguard-android.txt"))
        }
    }

    externalNativeBuild {
        cmake {
            path = file("CMakeLists.txt")
        }
    }

    buildFeatures {
        prefab = true
    }

}

dependencies {
    implementation("androidx.core:core:1.9.0")
    implementation("androidx.appcompat:appcompat:1.5.1")
    implementation("androidx.games:games-activity:1.2.2-alpha01")
}

