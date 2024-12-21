package com.gamemaker.game;

import org.libsdl.app.SDLActivity;

import android.widget.Toast;
import android.util.Log;
import android.os.Bundle;

import androidx.annotation.NonNull;
import com.google.android.gms.ads.AdError;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.FullScreenContentCallback;
import com.google.android.gms.ads.LoadAdError;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.interstitial.InterstitialAd;
import com.google.android.gms.ads.interstitial.InterstitialAdLoadCallback;

public class MyGame extends SDLActivity
{
    private InterstitialAd IAd;
    private static final String TAG = "MyGame";

    // JNI interface function
    public void loadAd()
    {
        Log.i(TAG, "Attempting to load an ad.");
        if (IAd != null) {
            Log.i(TAG, "An ad is already loaded.");
            return;
        }

        AdRequest adRequest = new AdRequest.Builder().build();

        InterstitialAd.load(this, "ca-app-pub-3940256099942544/1033173712", adRequest,
                new InterstitialAdLoadCallback() {
                    @Override
                    public void onAdLoaded(@NonNull InterstitialAd interstitialAd) {
                        // The mInterstitialAd reference will be null until
                        // an ad is loaded.

                        IAd = interstitialAd;
                        NonAdLoaded();

                        IAd.setFullScreenContentCallback(new FullScreenContentCallback() {
                            @Override
                            public void onAdClicked() {
                                NonAdClicked();
                            }
                            // called when ad is dismissed
                            @Override
                            public void onAdDismissedFullScreenContent() {
                                NonAdDismissedFullScreenContent();
                                IAd = null;
                                loadAd();
                            }

                            @Override
                            public void onAdFailedToShowFullScreenContent(AdError adError) {
                                NonAdFailedToShowFullScreenContent();
                                IAd = null;
                            }

                            @Override
                            public void onAdImpression() {
                                NonAdImpression();
                            }

                            @Override
                            public void onAdShowedFullScreenContent() {
                                NonAdShowedFullScreenContent();
                            }
                        });
                    }

                    @Override
                    public void onAdFailedToLoad(@NonNull LoadAdError loadAdError) {
                        NonAdFailedToLoad();
                        // Handle the error
                        Log.d(TAG, loadAdError.toString());
                        IAd = null;
                    }
                });
    }

    // JNI interface function
    public void playAd()
    {
        if (IAd != null) {
            IAd.show(this);
        } else {
            Toast.makeText(this, "No ads are available at this time, try again later.", Toast.LENGTH_SHORT).show();
            Log.d("TAG", "The interstitial ad wasn't ready yet.");
            loadAd();
        }
    }

    public int initializeAdSystem()
    {
        MobileAds.initialize(this, initializationStatus -> {});

        /*
        com.google.android.gms.ads.MobileAds = com.google.android.gms.internal.ads.zzblv@2e6f1eb
        com.google.ads.mediation.vungle.VungleMediationAdapter = com.google.android.gms.internal.ads.zzblv@cccb48,
        com.google.ads.mediation.applovin.AppLovinMediationAdapter = com.google.android.gms.internal.ads.zzblv@ed2ade1
        com.google.ads.mediation.adcolony.AdColonyMediationAdapter = com.google.android.gms.internal.ads.zzblv@db4c706
        com.google.ads.mediation.tapjoy.TapjoyMediationAdapter = com.google.android.gms.internal.ads.zzblv@f40c1c7
        */

        if(MobileAds.getInitializationStatus().getAdapterStatusMap().get("com.google.android.gms.ads.MobileAds").getInitializationState().toString().equals("READY")){
            // preloading ad cause why not
            runOnUiThread(() -> loadAd());

            // succeed
            return 0;
        }
        // failed
        return 1;
    }

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // calling the entry function
        JNILoaderfunc();
    }

    // Ad loaded callback methods
    protected native void NonAdLoaded();
    protected native void NonAdFailedToLoad();
    protected native void NonAdClicked();
    protected native void NonAdDismissedFullScreenContent();
    protected native void NonAdFailedToShowFullScreenContent();
    protected native void NonAdImpression();
    protected native void NonAdShowedFullScreenContent();

    protected native void JNILoaderfunc();
}