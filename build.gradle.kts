buildscript {
    repositories {
       google()
       mavenCentral()
    }

    dependencies {
        classpath("com.android.tools.build:gradle:8.2.0")
        classpath("org.jetbrains.kotlin:kotlin-gradle-plugin:1.7.21")
    }

}

allprojects {
    repositories {
        google()
        mavenCentral()
    }

}
