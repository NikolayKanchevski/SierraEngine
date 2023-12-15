package com.sierra.${APPLICATION_NAME}

import android.os.Bundle
import com.google.androidgamesdk.GameActivity

class MainActivity : GameActivity()
{

    override fun onCreate(savedInstanceState: Bundle?)
    {
        supportActionBar?.hide()
        super.onCreate(savedInstanceState)
    }

    // Load application library
    companion object { init { System.loadLibrary("${APPLICATION_NAME}") } }

}