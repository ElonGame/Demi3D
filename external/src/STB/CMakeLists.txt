project(STB)

# source files
file (GLOB C_FILES *.c)
file (GLOB H_FILES *.h)
set (SOURCE_FILES ${C_FILES} ${H_FILES})

add_library(STB STATIC ${SOURCE_FILES})

if (APPLE)
 set_target_properties(STB PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO")

 if(DEMI_BUILD_PLATFORM_APPLE_IOS)
  set_target_properties(STB PROPERTIES IPHONEOS_DEPLOYMENT_TARGET MIN_IOS_VERSION )
  set_target_properties(STB PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET[arch=arm64] "7.0" )
  set_target_properties(STB PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
 endif()
endif()

set(STB_INCLUDE_DIR "${STB_SOURCE_DIR}" CACHE PATH "" FORCE)
set(STB_LIBRARY_DBG "STB" CACHE STRING "" FORCE)
set(STB_LIBRARY_REL "STB" CACHE STRING "" FORCE)
mark_as_advanced(STB_INCLUDE_DIR STB_LIBRARY_DBG STB_LIBRARY_REL)
