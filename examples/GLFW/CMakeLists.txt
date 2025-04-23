# common settings required to build examples
include_directories (${GLFW_INCLUDE_DIRS})

# build all targets
foreach (example haptech-prototype 01-mydevice 02-multi-devices 03-analytics 04-shapes 05-fonts 06-images 07-mouse-select 08-shaders 09-magnets 10-oring 11-effects 12-polygons 13-primitives 14-textures 15-paint 16-friction 17-shading 18-endoscope 19-space 20-map 21-object 22-chrome 23-tooth 24-turntable 25-sounds 26-video 27-multiframes 28-voxel-basic 29-voxel-isosurface 30-voxel-colormap 31-pointcloud)
  message("chai3d example added - ${example}")
  file(GLOB source ${example}/*.cpp)

  # Special handling for haptech-prototype
  if (${example} STREQUAL "haptech-prototype")
    # Add the gRPC-generated sources
    list(APPEND source
      ${example}/franka_trial.pb.cc
      ${example}/franka_trial.grpc.pb.cc
    )
  endif()

  add_executable(${example} ${source})

  # Link libraries (extra ones for haptech-prototype)
  if (${example} STREQUAL "haptech-prototype")
    target_link_libraries(${example}
      ${CHAI3D_LIBRARIES}
      ${GLFW_LIBRARIES}
      grpc++
      grpc
      protobuf
      pthread
    )
  else()
    target_link_libraries(${example} ${CHAI3D_LIBRARIES} ${GLFW_LIBRARIES})
  endif()

  # OS specific adjustments
  if (${CMAKE_SYSTEM_NAME} MATCHES Darwin)
    add_custom_command(TARGET ${example} POST_BUILD
      COMMAND Rez -append ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../resources/icons/chai3d.rsrc -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${example}
      COMMAND SetFile -a C ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${example}
      VERBATIM
    )
  endif()
endforeach()
