#ifndef SMALLNAME_SDL_ADS_GLUE_CODE_HPP
#define SMALLNAME_SDL_ADS_GLUE_CODE_HPP

#include <jni.h>
#include <android/looper.h>
#include <android/log.h>
#include <unistd.h>
#include <thread>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "SDL_Ads", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "SDL_Ads", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "SDL_Ads", __VA_ARGS__)
#define JAVA extern "C" JNIEXPORT void JNICALL

static ALooper* mainThread;
static int messagePipe[2];

// base
// override the methods to change the behaviour
class AdLoadCallback {
private:
    // pointer to base class
    AdLoadCallback *smart_ptr;

public:
    explicit AdLoadCallback(AdLoadCallback *ptr = nullptr)
    {
        smart_ptr = ptr;
    }

    virtual void onAdLoaded()                           { LOGI("Ad loaded."); }
    virtual void onAdFailedToLoad()                     { LOGE("There was an error while loading an Ad."); }
    virtual void onAdClicked()                          { LOGD("Ad was clicked."); }
    virtual void onAdDismissedFullScreenContent()       { LOGD("Ad dismissed fullscreen content."); }
    virtual void onAdFailedToShowFullScreenContent()    { LOGE("Ad failed to show fullscreen content."); }
    virtual void onAdImpression()                       { LOGD("Ad recorded an impression."); }
    virtual void onAdShowedFullScreenContent()          { LOGD("Ad showed fullscreen content."); }

    AdLoadCallback* operator->()
    {
        return smart_ptr;
    }

    void selfOverride(AdLoadCallback *_obj)
    {
        smart_ptr = _obj;
    }

    ~AdLoadCallback()
    {
        // frees the pointer
        delete smart_ptr;
    }
};
// FREEEE!!!
AdLoadCallback mainAdCallback = AdLoadCallback(new AdLoadCallback());

// all the java methods called are in this class
class JavaMethods {
private:
    JavaVM *g_JVM;
    jobject m_MyGameHandler;
    jmethodID m_loadAd;
    jmethodID m_playAd;
    jmethodID m_initializeAdSystem;

public:
    JavaMethods() = default;

    JavaMethods(JavaVM *p_jvm, jobject handler, jmethodID methodarr[]) {
        g_JVM = p_jvm;
        m_MyGameHandler = handler;

        m_loadAd = methodarr[0];
        m_playAd = methodarr[1];
        m_initializeAdSystem = methodarr[2];
    }

    void cloadAd() {
        JNIEnv *env;
        LOGI("CALLING MAIN FUNCTION IN MAIN THREAD. ATTEMPTING TO LOAD AN AD.");
        env->CallVoidMethod(m_MyGameHandler, m_loadAd);
    }

    void cplayAd() {
        JNIEnv *env;
        g_JVM->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
        env->CallVoidMethod(m_MyGameHandler, m_playAd);
    }

    void cinitializeAdSystem() {
        JNIEnv *env;
        g_JVM->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
        jint res = env->CallIntMethod(m_MyGameHandler, m_initializeAdSystem);

        if (res == 0) {
            LOGI("Ad system initialized successfully");
        } else if (res == 1) {
            LOGI("Ad system failed to initialize");
        }
    }
}j_instance = JavaMethods();

// Enqueues a code to the listener
void EnqueueToPipe(char msg)
{
    write(messagePipe[1], &msg, 1);
}

static int looperlistener(int fd, int events, void* data) {
    char msg;
    read(fd, &msg, 1); // read message from pipe
    if(msg == 2)
    {
        j_instance.cplayAd();
    }
    else if(msg == 3)
    {
        j_instance.cloadAd();
    }
    else if(msg == 1)
    {
        j_instance.cinitializeAdSystem();
    }
    else if(msg == 0)
    {
        return 0; // exit
    }
    return 1; // continue listening for events
}

JAVA Java_com_gamemaker_game_MyGame_NonAdLoaded(JNIEnv* env, jobject thiz)                          { mainAdCallback->onAdLoaded(); }
JAVA Java_com_gamemaker_game_MyGame_NonAdFailedToLoad(JNIEnv* env, jobject thiz)                    { mainAdCallback->onAdFailedToLoad(); }
JAVA Java_com_gamemaker_game_MyGame_NonAdClicked(JNIEnv* env, jobject thiz)                         { mainAdCallback->onAdClicked(); }
JAVA Java_com_gamemaker_game_MyGame_NonAdDismissedFullScreenContent(JNIEnv* env, jobject thiz)      { mainAdCallback->onAdDismissedFullScreenContent(); }
JAVA Java_com_gamemaker_game_MyGame_NonAdFailedToShowFullScreenContent(JNIEnv* env, jobject thiz)   { mainAdCallback->onAdFailedToShowFullScreenContent(); }
JAVA Java_com_gamemaker_game_MyGame_NonAdImpression(JNIEnv* env, jobject thiz)                      { mainAdCallback->onAdImpression(); }
JAVA Java_com_gamemaker_game_MyGame_NonAdShowedFullScreenContent(JNIEnv* env, jobject thiz)         { mainAdCallback->onAdShowedFullScreenContent(); }

// main function called from main thread to load everything
// Entry function
// Do not call
JAVA Java_com_gamemaker_game_MyGame_JNILoaderfunc(JNIEnv* env, jobject thiz)
{
    JavaVM *temp_jvm = nullptr;
    jobject temp_handler = nullptr;
    jmethodID temp_methodarr[3];

    mainThread = ALooper_forThread();
    if(mainThread == NULL)
    {
        LOGI("COULDNT GET MAIN THREAD");
    }
    ALooper_acquire(mainThread);
    pipe(messagePipe);
    // this will be called on main thread
    ALooper_addFd(mainThread, messagePipe[0],0, ALOOPER_EVENT_INPUT, looperlistener, nullptr);

    env->GetJavaVM(&temp_jvm);

    temp_handler = env->NewGlobalRef(thiz);
    // get the class from the jvm
    jclass objClass = env->GetObjectClass(thiz);

    if(objClass)
    {
        // idk why we do this but it just gets a new global reference to the class we got
        jclass temp_class = reinterpret_cast<jclass>(env->NewGlobalRef(objClass));
        // loadAd
        temp_methodarr[0] = env->GetMethodID(temp_class, "loadAd", "()V");
        // playAd
        temp_methodarr[1] = env->GetMethodID(temp_class, "playAd", "()V");
        // initializeAdSystem
        temp_methodarr[2] = env->GetMethodID(temp_class, "initializeAdSystem", "()I");
    }

    j_instance = JavaMethods(temp_jvm, temp_handler, temp_methodarr);
}

#endif //SMALLNAME_SDL_ADS_GLUE_CODE_HPP
