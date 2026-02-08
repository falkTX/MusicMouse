# MusicMouse on DPF
# Copyright (C) 2026 Filipe Coelho <falktx@falktx.com>
# SPDX-License-Identifier: ISC

cmake_minimum_required(VERSION 3.7...3.31)

set(CMAKE_CXX_STANDARD 11)

set(NAME imgui-demo-plugin)
project(MusicMouse)

add_subdirectory(dpf)

dpf_add_plugin(MusicMouse
  TARGETS clap lv2 vst2 vst3 jack
  FILES_DSP
      src/PluginDSP.cpp
  FILES_UI
      src/PluginUI.cpp
  UI_TYPE external
  NO_SHARED_RESOURCES
  USE_WEB_VIEW
)

target_include_directories(MusicMouse
  PUBLIC
    src
)
