cmake_minimum_required(VERSION 3.20)

if(CONFIG_IRONHAND_TARKOV_DISPLAY)
    target_sources(app PRIVATE src/display/tarkov_widget.c)
    target_include_directories(app PRIVATE src/display)
endif()
