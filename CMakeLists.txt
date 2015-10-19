#cmake_minimum_required(VERSION 2.8.12)



project(test)
#project(demo_ipm_test)

find_package(OpenCV REQUIRED)
find_package(Hiredis REQUIRED)
#find_package(LibEV REQUIRED)
find_library(LIBEV_LIBRARIES NAMES ev)

if (EVENT_INCLUDE_DIR AND EVENT_LIBRARY)
  # Already in cache, be silent
  set(EVENT_FIND_QUIETLY TRUE)
endif (EVENT_INCLUDE_DIR AND EVENT_LIBRARY)
 
find_path(EVENT_INCLUDE_DIR event.h
  PATHS /usr/include
  PATH_SUFFIXES event
)
 
find_library(EVENT_LIBRARY
  NAMES event
  PATHS /usr/lib /usr/local/lib
)
 
set(EVENT_LIBRARIES ${EVENT_LIBRARY} )
 
add_definitions(-DLIBNET_LIL_ENDIAN)
 
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EVENT
  DEFAULT_MSG
  EVENT_INCLUDE_DIR
  EVENT_LIBRARIES
)
 
mark_as_advanced(EVENT_INCLUDE_DIR EVENT_LIBRARY)



include_directories(${OpenCV_INCLUDE_DIRS} ${HIREDIS_INCLUDE_DIR} /usr/local/include/hiredis)

#add_executable(demo road_detect.cpp)#mcv.cc CameraInfoOpt.c InversePerspectiveMapping.cc demo.cpp)
#add_executable(demo cmdline.c LaneDetectorOpt.c LaneDetector.cc mcv.cc CameraInfoOpt.c InversePerspectiveMapping.cc demo.cpp)
#add_executable(demo_ipm_test cmdline.c LaneDetectorOpt.c LaneDetector.cc mcv.cc CameraInfoOpt.c InversePerspectiveMapping.cc demo_ipm_test.cpp)
#target_link_libraries(demo ${OpenCV_LIBS} ${HIREDIS_LIBRARIES})
#target_link_libraries(demo_ipm_test ${OpenCV_LIBS} ${HIREDIS_LIBRARIES})

add_executable(test test.cpp)
target_link_libraries(test ${OpenCV_LIBS} ${HIREDIS_LIBRARIES} ${LIBEV_LIBRARIES} ${EVENT_LIBRARIES})


add_executable(test_redis test_redis.cpp)
target_link_libraries(test_redis ${OpenCV_LIBS} ${HIREDIS_LIBRARIES} ${EVENT_LIBRARIES}) # ${LIBEV_LIBRARIES} ${EVENT_LIBRARIES})

add_executable(redis_egomotion_reader redis_egomotion_reader.cpp debug_print_common.cpp)
target_link_libraries(redis_egomotion_reader ${OpenCV_LIBS} ${HIREDIS_LIBRARIES} ${EVENT_LIBRARIES}) # ${LIBEV_LIBRARIES} ${EVENT_LIBRARIES})
