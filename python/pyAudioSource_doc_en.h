#pragma once

#include <Python.h>

// enable
PyDoc_STRVAR(AudioSource_enable_doc,
             "Enable/disable.\n"
             "Type: bool\n");

// playOnEnabled
PyDoc_STRVAR(AudioSource_playOnEnabled_doc,
             "Auto play audio when enable.\n"
             "Type: bool\n");

// stream
PyDoc_STRVAR(AudioSource_stream_doc,
             "Streamed or fully loaded on memory.\n"
             "Type: bool\n");

// loop
PyDoc_STRVAR(AudioSource_loop_doc,
             "Should audio to be looped.\n"
             "Type: bool\n");

// singleInstance
PyDoc_STRVAR(AudioSource_singleInstance_doc,
             "Should audio to be single instance (stop previous instance before playing new instance).\n"
             "Type: bool\n");

// path
PyDoc_STRVAR(AudioSource_path_doc,
             "Path to the audio file.\n"
             "Type: string\n");

// volume
PyDoc_STRVAR(AudioSource_volume_doc,
             "Audio volume (0.f - 1.f).\n"
             "Type: float\n");

// pan
PyDoc_STRVAR(AudioSource_pan_doc,
             "Audio panning (-1.f: left, 0: center, 1.f: right).\n"
             "Type: float\n");

// velocity
PyDoc_STRVAR(AudioSource_velocity_doc,
             "Audio velocity.\n"
             "Type: Vec3\n");

// minDistance
PyDoc_STRVAR(AudioSource_minDistance_doc,
             "Audio min distance.\n"
             "Type: float\n");

// maxDistance
PyDoc_STRVAR(AudioSource_maxDistance_doc,
             "Audio max distance.\n"
             "Type: float\n");

// attenuationModel
PyDoc_STRVAR(AudioSource_attenuationModel_doc,
             "Audio attenuation model (0: no attenuation, 1: inverse distance, 2: linear distance, 3: exponential distance).\n"
             "Type: float\n");

// attenuationFactor
PyDoc_STRVAR(AudioSource_attenuationFactor_doc,
             "Audio attenuation roll-off factor.\n"
             "Type: float\n");

// dopplerFactor
PyDoc_STRVAR(AudioSource_dopplerFactor_doc,
             "Audio doppler factor.\n"
             "Type: float\n");

// play
PyDoc_STRVAR(AudioSource_play_doc,
             "Play audio.\n");

// pause
PyDoc_STRVAR(AudioSource_pause_doc,
             "Pause audio.\n");

// stop
PyDoc_STRVAR(AudioSource_stop_doc,
             "Stop audio.\n");

// resume
PyDoc_STRVAR(AudioSource_resume_doc,
             "Resume audio.\n");

// seek
PyDoc_STRVAR(AudioSource_seek_doc,
             "Seek audio by seconds.\n"
             "Parameters\n"
             "----------\n"
             "    seconds : float\n"
             "        seek time in second\n");

// isPaused
PyDoc_STRVAR(AudioSource_isPaused_doc,
             "Check whether the audio is paused.\n");

// isStopped
PyDoc_STRVAR(AudioSource_isStopped_doc,
             "Check whether the audio is stopped.\n");
