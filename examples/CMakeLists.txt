cmake_minimum_required(VERSION 3.27)

if(INSTRUMENTCONTROL_BUILD_VISA)
  add_subdirectory(VISA)
endif()

if(INSTRUMENTCONTROL_BUILD_VISACLIENT)
  add_subdirectory(VISAClient)
endif()
