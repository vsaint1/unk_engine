package com.unkengine.app;

import android.os.Bundle;

import org.libsdl.app.SDLActivity;

public class MainActivity extends SDLActivity {


    @Override
    protected String[] getLibraries() {

        return new String[]{
                "SDL3",
                "SDL3_ttf",
                "SDL3_mixer",
//                "SDL3_image",
                "unk_engine"
        };
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }
}
