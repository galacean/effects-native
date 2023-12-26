cmake_minimum_required(VERSION 3.4.1)

add_compile_options(-std=c++14)
add_compile_options(-fno-rtti)

message(STATUS "BUILD_PLAYGROUND=${BUILD_PLAYGROUND}")
if (${BUILD_PLAYGROUND})
    MESSAGE(STATUS "keep function signature")
    set(CMAKE_CXX_VISIBILITY_PRESET default)
    add_definitions(-DBUILD_PLAYGROUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections")
else ()
    MESSAGE(STATUS "hide function signature")
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections")
endif ()

set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    -Wl,--gc-sections")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--gc-sections")

MESSAGE(STATUS "BUILD ANDROID SO")

if (DEFINED BUILD_OSS)
    add_definitions(-DBUILD_MARS_OSS)
endif ()

set(PLATFORM_PART_LIB
    android
    log
    EGL
    GLESv2
    jnigraphics)

set(LIB_TYPE SHARED)

include_directories(
    ${CMAKE_SOURCE_DIR}
)
if (NOT DEFINED BUILD_OSS)
    include_directories(
        ${CMAKE_SOURCE_DIR}/third-party/headers
        ${CMAKE_SOURCE_DIR}/../../core/third-party/headers
    )
endif ()


add_library(marsnative ${LIB_TYPE}
    ${CMAKE_SOURCE_DIR}/ticker.cpp

    ${CMAKE_SOURCE_DIR}/load/loader.cpp

    ${CMAKE_SOURCE_DIR}/math/camera.cpp
    ${CMAKE_SOURCE_DIR}/math/mat3.cpp
    ${CMAKE_SOURCE_DIR}/math/mat4.cpp
    ${CMAKE_SOURCE_DIR}/math/math_util.cpp
    ${CMAKE_SOURCE_DIR}/math/mersenne_twister.cpp
    ${CMAKE_SOURCE_DIR}/math/quaternion.cpp
    ${CMAKE_SOURCE_DIR}/math/transform.cpp
    ${CMAKE_SOURCE_DIR}/math/translate.cpp
    ${CMAKE_SOURCE_DIR}/math/vec2.cpp
    ${CMAKE_SOURCE_DIR}/math/vec3.cpp
    ${CMAKE_SOURCE_DIR}/math/vec4.cpp

    ${CMAKE_SOURCE_DIR}/math/value/bezier_value_getter.cpp
    ${CMAKE_SOURCE_DIR}/math/value/curve_value_getter.cpp
    ${CMAKE_SOURCE_DIR}/math/value/gradient_value_getter.cpp
    ${CMAKE_SOURCE_DIR}/math/value/linear_value_getter.cpp
    ${CMAKE_SOURCE_DIR}/math/value/line_segments_value_getter.cpp
    ${CMAKE_SOURCE_DIR}/math/value/path_value_getter.cpp
    ${CMAKE_SOURCE_DIR}/math/value/random_value_getter.cpp
    ${CMAKE_SOURCE_DIR}/math/value/static_value_getter.cpp
    ${CMAKE_SOURCE_DIR}/math/value/value_getter_utils.cpp

    ${CMAKE_SOURCE_DIR}/filters/bloom_filter_define.cpp
    ${CMAKE_SOURCE_DIR}/filters/delay_filter_define.cpp
    ${CMAKE_SOURCE_DIR}/filters/distortion_filter_define.cpp
    ${CMAKE_SOURCE_DIR}/filters/gaussian_filter_define.cpp
    ${CMAKE_SOURCE_DIR}/filters/filter_utils.cpp

    ${CMAKE_SOURCE_DIR}/platform/android/android_impl.cpp
    ${CMAKE_SOURCE_DIR}/platform/android/cov_util.cpp
    ${CMAKE_SOURCE_DIR}/platform/android/egl_pbuffer_context.cpp
    ${CMAKE_SOURCE_DIR}/platform/android/egl_window_context.cpp
    ${CMAKE_SOURCE_DIR}/platform/android/jni_bridge.cpp
    ${CMAKE_SOURCE_DIR}/platform/android/mars_offscreen_surface.cpp
    ${CMAKE_SOURCE_DIR}/platform/android/mars_player_jni.cpp
    ${CMAKE_SOURCE_DIR}/platform/android/mars_surface_android.cpp
    ${CMAKE_SOURCE_DIR}/platform/android/jni_java_impls/jni_mars_native_resource_loader.cpp

    ${CMAKE_SOURCE_DIR}/model/common.cpp
    ${CMAKE_SOURCE_DIR}/model/model_plugin.cpp
    ${CMAKE_SOURCE_DIR}/model/model_tree_plugin.cpp
    ${CMAKE_SOURCE_DIR}/model/model_vfx_item.cpp

    ${CMAKE_SOURCE_DIR}/model/renderer/m_camera.cpp
    ${CMAKE_SOURCE_DIR}/model/renderer/m_entity.cpp
    ${CMAKE_SOURCE_DIR}/model/renderer/m_geometry.cpp
    ${CMAKE_SOURCE_DIR}/model/renderer/m_light.cpp
    ${CMAKE_SOURCE_DIR}/model/renderer/m_mesh.cpp
    ${CMAKE_SOURCE_DIR}/model/renderer/m_scene_manager.cpp
    ${CMAKE_SOURCE_DIR}/model/renderer/m_skybox.cpp
    ${CMAKE_SOURCE_DIR}/model/renderer/m_transform.cpp
    ${CMAKE_SOURCE_DIR}/model/renderer/animation/interpolation_sampler.cpp
    ${CMAKE_SOURCE_DIR}/model/renderer/animation/m_animation.cpp
    ${CMAKE_SOURCE_DIR}/model/renderer/animation/m_skin.cpp
    ${CMAKE_SOURCE_DIR}/model/renderer/material/m_material_base.cpp
    ${CMAKE_SOURCE_DIR}/model/renderer/material/m_material_pbr.cpp
    ${CMAKE_SOURCE_DIR}/model/renderer/material/m_material_skybox.cpp

    ${CMAKE_SOURCE_DIR}/model/sceneData/data/model_data.cpp
    ${CMAKE_SOURCE_DIR}/model/sceneData/decoder/model_content_decoder.cpp
    ${CMAKE_SOURCE_DIR}/model/shader/shader_libs.cpp
    ${CMAKE_SOURCE_DIR}/model/utility/skybox_utils.cpp

    ${CMAKE_SOURCE_DIR}/player/comp_vfx_item.cpp
    ${CMAKE_SOURCE_DIR}/player/composition.cpp
    ${CMAKE_SOURCE_DIR}/player/mars_native_data.cpp
    ${CMAKE_SOURCE_DIR}/player/mars_native_player.cpp
    ${CMAKE_SOURCE_DIR}/player/mars_player_mgr.cpp
    ${CMAKE_SOURCE_DIR}/player/mars_player.cpp
    ${CMAKE_SOURCE_DIR}/player/plugin_system.cpp
    ${CMAKE_SOURCE_DIR}/player/vfx_item.cpp

    ${CMAKE_SOURCE_DIR}/plugin/cal/calculate_group.cpp
    ${CMAKE_SOURCE_DIR}/plugin/cal/calculate_item.cpp
    ${CMAKE_SOURCE_DIR}/plugin/cal/calculate_loader.cpp
    ${CMAKE_SOURCE_DIR}/plugin/cal/calculate_vfx_item.cpp

    ${CMAKE_SOURCE_DIR}/plugin/interact/interact_vfx_item.cpp

    ${CMAKE_SOURCE_DIR}/plugin/particle/link_chain.cpp
    ${CMAKE_SOURCE_DIR}/plugin/particle/particle_loader.cpp
    ${CMAKE_SOURCE_DIR}/plugin/particle/particle_mesh.cpp
    ${CMAKE_SOURCE_DIR}/plugin/particle/particle_system.cpp
    ${CMAKE_SOURCE_DIR}/plugin/particle/particle_vfx_item.cpp

    ${CMAKE_SOURCE_DIR}/plugin/particle/shape/cone.cpp
    ${CMAKE_SOURCE_DIR}/plugin/particle/shape/donut.cpp
    ${CMAKE_SOURCE_DIR}/plugin/particle/shape/earcut.cpp
    ${CMAKE_SOURCE_DIR}/plugin/particle/shape/geometry_2d.cpp
    ${CMAKE_SOURCE_DIR}/plugin/particle/shape/shape.cpp
    ${CMAKE_SOURCE_DIR}/plugin/particle/shape/sphere.cpp

    ${CMAKE_SOURCE_DIR}/plugin/sprite/sprite_group.cpp
    ${CMAKE_SOURCE_DIR}/plugin/sprite/sprite_item.cpp
    ${CMAKE_SOURCE_DIR}/plugin/sprite/sprite_loader.cpp
    ${CMAKE_SOURCE_DIR}/plugin/sprite/sprite_mesh.cpp
    ${CMAKE_SOURCE_DIR}/plugin/sprite/sprite_vfx_item.cpp
    ${CMAKE_SOURCE_DIR}/plugin/sprite/filter_sprite_vfx_item.cpp

    ${CMAKE_SOURCE_DIR}/plugin/arTrack/ar_track_loader.cpp
    ${CMAKE_SOURCE_DIR}/plugin/arTrack/ar_track_vfx_item.cpp

    ${CMAKE_SOURCE_DIR}/plugin/tree/tree_item.cpp
    ${CMAKE_SOURCE_DIR}/plugin/tree/tree_vfx_item.cpp

    ${CMAKE_SOURCE_DIR}/ri/render_base.cpp
    ${CMAKE_SOURCE_DIR}/ri/utils.cpp

    ${CMAKE_SOURCE_DIR}/ri/backend/opengl/gl_api.cpp
    ${CMAKE_SOURCE_DIR}/ri/backend/opengl/gl_frame_buffer.cpp
    ${CMAKE_SOURCE_DIR}/ri/backend/opengl/gl_geometry.cpp
    ${CMAKE_SOURCE_DIR}/ri/backend/opengl/gl_gpu_buffer.cpp
    ${CMAKE_SOURCE_DIR}/ri/backend/opengl/gl_gpu_capability.cpp
    ${CMAKE_SOURCE_DIR}/ri/backend/opengl/gl_gpu_renderer.cpp
    ${CMAKE_SOURCE_DIR}/ri/backend/opengl/gl_material.cpp
    ${CMAKE_SOURCE_DIR}/ri/backend/opengl/gl_program.cpp
    ${CMAKE_SOURCE_DIR}/ri/backend/opengl/gl_render_buffer.cpp
    ${CMAKE_SOURCE_DIR}/ri/backend/opengl/gl_shader_library.cpp
    ${CMAKE_SOURCE_DIR}/ri/backend/opengl/gl_state.cpp
    ${CMAKE_SOURCE_DIR}/ri/backend/opengl/gl_texture.cpp

    ${CMAKE_SOURCE_DIR}/ri/render/mars_geometry.cpp
    ${CMAKE_SOURCE_DIR}/ri/render/mars_material.cpp
    ${CMAKE_SOURCE_DIR}/ri/render/mars_mesh.cpp
    ${CMAKE_SOURCE_DIR}/ri/render/mars_player_render_frame.cpp
    ${CMAKE_SOURCE_DIR}/ri/render/mars_renderer.cpp
    ${CMAKE_SOURCE_DIR}/ri/render/mars_renderer_ext.cpp
    ${CMAKE_SOURCE_DIR}/ri/render/mars_texture.cpp
    ${CMAKE_SOURCE_DIR}/ri/render/render_frame.cpp
    ${CMAKE_SOURCE_DIR}/ri/render/render_frame_internal.cpp

    ${CMAKE_SOURCE_DIR}/ri/render/render_pass_color_attachment.cpp
    ${CMAKE_SOURCE_DIR}/ri/render/render_pass.cpp

    ${CMAKE_SOURCE_DIR}/sceneData/mars_data_base.cpp
    ${CMAKE_SOURCE_DIR}/sceneData/scene_data_decoder.cpp
    ${CMAKE_SOURCE_DIR}/sceneData/scene_data_decoder_new.cpp

    ${CMAKE_SOURCE_DIR}/sceneData/data/sprite_mesh_geometry.cpp

    ${CMAKE_SOURCE_DIR}/sceneData/decoder/common_properties_decoder.cpp
    ${CMAKE_SOURCE_DIR}/sceneData/decoder/composition_decoder.cpp
    ${CMAKE_SOURCE_DIR}/sceneData/decoder/interact_content_decoder.cpp
    ${CMAKE_SOURCE_DIR}/sceneData/decoder/item_decoder.cpp
    ${CMAKE_SOURCE_DIR}/sceneData/decoder/particle_content_decoder.cpp
    ${CMAKE_SOURCE_DIR}/sceneData/decoder/plugin_content_decoder.cpp
    ${CMAKE_SOURCE_DIR}/sceneData/decoder/sprite_content_decoder.cpp
    ${CMAKE_SOURCE_DIR}/sceneData/decoder/filter_content_decoder.cpp

    ${CMAKE_SOURCE_DIR}/video/mars_video_texture.cpp
    ${CMAKE_SOURCE_DIR}/video/video_image_data.cpp
    ${CMAKE_SOURCE_DIR}/video/android/video_buffer_processor.cpp
    
    ${CMAKE_SOURCE_DIR}/util/color_utils.cpp
    ${CMAKE_SOURCE_DIR}/util/constant.cpp
    ${CMAKE_SOURCE_DIR}/util/image_loader.cpp
    ${CMAKE_SOURCE_DIR}/util/log_util.cpp
    ${CMAKE_SOURCE_DIR}/util/mars_thread.cpp
    ${CMAKE_SOURCE_DIR}/util/util.cpp
    ${CMAKE_SOURCE_DIR}/util/vfx_value_getter.cpp

    ${CMAKE_SOURCE_DIR}/extensions/mars_ext_position_path_modifier.cpp
    ${CMAKE_SOURCE_DIR}/extensions/morph/custom_morph.cpp
    ${CMAKE_SOURCE_DIR}/extensions/jni/mars_ext_position_path_modifier_jni.cpp
    ${CMAKE_SOURCE_DIR}/extensions/jni/mars_ext_custom_morph_jni.cpp

    ${CMAKE_SOURCE_DIR}/adapter/mars_video_player.cpp
)

if(SANITIZE)
    add_compile_options(-g)

    set(ARCH_STR ${CMAKE_ANDROID_ARCH})
    if ("${CMAKE_ANDROID_ARCH_ABI}" STREQUAL "arm64-v8a")
        set(ARCH_STR "aarch64")
    elseif ("${CMAKE_ANDROID_ARCH_ABI}" STREQUAL "armeabi")
        set(ARCH_STR "arm")
    elseif ("${CMAKE_ANDROID_ARCH_ABI}" STREQUAL "armeabi-v7a")
        set(ARCH_STR "arm")
    endif()

    get_filename_component(HINT_PATH ${ANDROID_C_COMPILER} DIRECTORY)
    message(WARNING "HINT_PATH is: ${HINT_PATH}")

    ### ubsan
    target_compile_options(marsnative PUBLIC -fsanitize=undefined -fno-sanitize-recover=undefined)
    set_target_properties(marsnative
            PROPERTIES LINK_FLAGS "-fsanitize=undefined -fno-sanitize-recover=undefined")

    file(GLOB UBSAN_GLOB "${HINT_PATH}/../lib64/clang/*/lib/linux")
    find_file(UBSAN
            NAMES libclang_rt.ubsan_standalone-${ARCH_STR}-android.so
            PATHS ${UBSAN_GLOB})
    get_filename_component(UBSAN_NAME ${UBSAN} NAME)
    set(UBSAN_NAME ${CMAKE_SOURCE_DIR}/../android/mars/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/${UBSAN_NAME})
    add_custom_command(
            TARGET marsnative PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${UBSAN} ${UBSAN_NAME})

    ### asan
    target_compile_options(marsnative PUBLIC -fsanitize=address -fno-omit-frame-pointer)
    set_target_properties(marsnative
            PROPERTIES LINK_FLAGS "-fsanitize=address")

    file(GLOB ASAN_GLOB "${HINT_PATH}/../lib64/clang/*/lib/linux")
    find_file(ASAN
            NAMES libclang_rt.asan-${ARCH_STR}-android.so
            PATHS ${ASAN_GLOB})
    get_filename_component(ASAN_NAME ${ASAN} NAME)
    set(ASAN_NAME ${CMAKE_SOURCE_DIR}/../android/mars/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/${ASAN_NAME})
    add_custom_command(
            TARGET marsnative PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${ASAN} ${ASAN_NAME})

    find_file(WRAP
            NAMES asan.sh
            HINTS ${HINT_PATH}/../../../../../wrap.sh)
    set(WRAP_NAME ${CMAKE_SOURCE_DIR}/../android/mars/src/main/resources/lib/${CMAKE_ANDROID_ARCH_ABI}/wrap.sh)
    add_custom_command(
            TARGET marsnative PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${WRAP} ${WRAP_NAME})
endif()

if (NOT DEFINED BUILD_OSS)
    set(THIRD_PARTY_LIBS
        ${CMAKE_SOURCE_DIR}/../../core/third-party/libs/${ANDROID_ABI}/libmfproducts4x.so
    )
endif ()

target_link_libraries(marsnative
        ${THIRD_PARTY_LIBS}
        ${PLATFORM_PART_LIB})
