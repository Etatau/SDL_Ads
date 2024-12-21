#ifndef SMALLNAME_SDL_ADS_HPP
#define SMALLNAME_SDL_ADS_HPP
#include <SDL_Ads_glue_code.hpp>

// multilevel include
class AdLoadCallbackMethod : public AdLoadCallback {};

void initializeAdSystem()
{
    EnqueueToPipe(1);
}

void loadAd()
{
    EnqueueToPipe(3);
}

void playAd()
{
    EnqueueToPipe(2);
}

void override_callback(AdLoadCallbackMethod *_method)
{
    mainAdCallback.selfOverride(_method);
}

#endif //SMALLNAME_SDL_ADS_HPP