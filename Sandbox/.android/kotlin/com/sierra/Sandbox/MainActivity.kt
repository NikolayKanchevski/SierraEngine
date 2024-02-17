package com.sierra.Sandbox

import android.os.Bundle
import com.google.androidgamesdk.GameActivity

class MainActivity : GameActivity()
{

    override fun onCreate(savedInstanceState: Bundle?)
    {
        supportActionBar?.hide()
        super.onCreate(savedInstanceState)
    }

    // Create application library
    companion object { init { System.loadLibrary("Sandbox") } }

}