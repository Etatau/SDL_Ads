settings for everything:

1) root build.gradle

set:
classpath 'com.android.tools.build:gradle:8.7.0'

2) app build.gradle

set:
// path to your entry java class
namespace "com.gamemaker.game"

// latest version of google mobile ads requires this as minimum sdk version
minSdkVersion 21

// for ndkBuild
arguments "APP_PLATFORM=android-21"

// for cmake
arguments "-DANDROID_PLATFORM=android-21", "-DANDROID_STL=c++_static"

// change this for both ndkBuild and cmake
// if you are going to run this in android studio
abiFilters 'x86'
// if you are going to run this on a device then
abiFilters arm64-v8a // for 64 bit devices
abiFilters armeabi-v7a // for 32 bit devices

// add this to dependencies
implementation 'com.google.android.gms:play-services-ads:23.6.0'

3) gradle.properties

set:
// add this
android.useAndroidX=true
// optionally you can add this if it cannot find java home
org.gradle.java.home = path_to_java_home

4) gradle-wrapper.properties

set:
distributionUrl=https\://services.gradle.org/distributions/gradle-8.9-bin.zip

5) jni/CMakeLists.txt

set:
// uncomment
add_subdirectory(SDL_image)

6) jni/Application.mk

set:
// uncomment
APP_STL := c++_shared

APP_PLATFORM=android-21

7) AndroidManifest.xml

set:
// this is the entry java class
<activity android:name=".MyGame"


// in between the <application> ... </application> tags
// the application ID is for testing purpose only and should be changed when releasing the actual app.
// sample ID provided by google
        <meta-data
            android:name="com.google.android.gms.ads.APPLICATION_ID"
            android:value="ca-app-pub-3940256099942544~3347511713"/>

8) jni/src/CMakeLists.txt and jni/src/Android.mk

// put your main application source file in there
// set in Android.mk LOCAL_SHARED_LIBRARIES := SDL3 SDL3_image








##########EXTRA##########

Follow the documentation provided here by SDL for building your project for android if you need additional information.
https://wiki.libsdl.org/SDL3/README/android

