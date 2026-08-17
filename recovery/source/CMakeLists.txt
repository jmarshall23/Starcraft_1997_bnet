cmake_minimum_required(VERSION 3.24)

project(StarCraftRecovery VERSION 0.0.1 LANGUAGES C CXX)

get_filename_component(STARCRAFT_RECOVERY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(LIBSMACKER_ROOT "${STARCRAFT_RECOVERY_ROOT}/thirdparty/libsmacker")
set(OPENAL_SOFT_ROOT "${STARCRAFT_RECOVERY_ROOT}/thirdparty/openal-soft")
set(IMGUI_ROOT "${STARCRAFT_RECOVERY_ROOT}/thirdparty/imgui")

if(NOT WIN32)
  message(FATAL_ERROR "The recovered target is Win32-only.")
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(FATAL_ERROR "Configure with a 32-bit generator (Visual Studio: -A Win32).")
endif()

include(CTest)

# Keep OpenAL Soft as a replaceable LGPL shared library beside the recovery
# executable. Only the native WASAPI output backend is needed by this target.
set(LIBTYPE SHARED CACHE STRING "OpenAL Soft library type" FORCE)
set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_TESTS OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_CONFIG OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_HRTF_DATA OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_UTILS OFF CACHE BOOL "" FORCE)
set(ALSOFT_UPDATE_BUILD_VERSION OFF CACHE BOOL "" FORCE)
set(ALSOFT_ENABLE_MODULES OFF CACHE BOOL "" FORCE)
set(ALSOFT_EAX OFF CACHE BOOL "" FORCE)
set(ALSOFT_EMBED_HRTF_DATA OFF CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_WASAPI ON CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_DSOUND OFF CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_WINMM OFF CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_WAVE OFF CACHE BOOL "" FORCE)
add_subdirectory(
  "${OPENAL_SOFT_ROOT}"
  "${CMAKE_CURRENT_BINARY_DIR}/thirdparty/openal-soft"
  EXCLUDE_FROM_ALL
)

find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  add_custom_target(verify_inputs
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/verify_inputs.py"
    COMMENT "Verifying licensed inputs without modifying them"
    VERBATIM
  )
endif()

find_program(DOTNET_EXECUTABLE dotnet)
if(DOTNET_EXECUTABLE)
  set(STARCRAFT_RECOVERY_SERVER_PROJECT
      "${CMAKE_CURRENT_SOURCE_DIR}/src/server/StarCraftRecoveryServer.csproj")
  set(STARCRAFT_RECOVERY_SERVER_EXE
      "${STARCRAFT_RECOVERY_ROOT}/StarCraftRecoveryServer.exe")
  set(STARCRAFT_RECOVERY_SERVER_PUBLISH_DIR
      "${CMAKE_CURRENT_BINARY_DIR}/server-publish")
  file(GLOB STARCRAFT_RECOVERY_SERVER_SOURCES CONFIGURE_DEPENDS
       "${CMAKE_CURRENT_SOURCE_DIR}/src/server/*.cs"
       "${CMAKE_CURRENT_SOURCE_DIR}/src/server/*.csproj")
  add_custom_command(
    OUTPUT "${STARCRAFT_RECOVERY_SERVER_EXE}"
    COMMAND "${DOTNET_EXECUTABLE}" publish
            "${STARCRAFT_RECOVERY_SERVER_PROJECT}"
            --configuration Release
            --runtime win-x64
            --self-contained false
            --output "${STARCRAFT_RECOVERY_SERVER_PUBLISH_DIR}"
            -p:PublishSingleFile=true
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${STARCRAFT_RECOVERY_SERVER_PUBLISH_DIR}/StarCraftRecoveryServer.exe"
            "${STARCRAFT_RECOVERY_SERVER_EXE}"
    DEPENDS ${STARCRAFT_RECOVERY_SERVER_SOURCES}
    COMMENT "Publishing the recovery Battle.net server"
    VERBATIM
  )
  add_custom_target(StarCraftRecoveryServer ALL
                    DEPENDS "${STARCRAFT_RECOVERY_SERVER_EXE}")
endif()

add_library(starcraft_core STATIC
  src/data/chk.cpp
  src/data/dat.cpp
  src/game/scenario.cpp
  src/gds/bitmap.cpp
  src/gds/font.cpp
  src/gds/grp.cpp
  src/gds/meta_bitmap.cpp
  src/gds/tileset.cpp
  src/lang/count.cpp
  src/lang/CUnitBuild.cpp
  src/lang/CUnitHarvest.cpp
  src/lang/CUnitInit.cpp
  src/lang/CUnitPathCollide.cpp
  src/lang/CUnitProtoss.cpp
  src/lang/CUnitTerran.cpp
  src/lang/CUnitZerg.cpp
  src/lang/damage.cpp
  src/lang/flingy.cpp
  src/lang/iscript.cpp
  src/lang/pathfinding.cpp
  src/lang/place_unit.cpp
  src/lang/tile2.cpp
  src/net/lockstep.cpp
  src/runtime/invariant.cpp
  src/runtime/storm.cpp
  src/staredit/mapcomn/creep.cpp
)

add_library(vendored_libsmacker STATIC "${LIBSMACKER_ROOT}/smacker.c")
target_include_directories(vendored_libsmacker PUBLIC "${LIBSMACKER_ROOT}")
set_target_properties(vendored_libsmacker PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED YES)
if(MSVC)
  target_compile_definitions(vendored_libsmacker PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

target_include_directories(starcraft_core PUBLIC include)
target_compile_features(starcraft_core PUBLIC cxx_std_17)
target_compile_definitions(starcraft_core PUBLIC WIN32_LEAN_AND_MEAN NOMINMAX)

if(MSVC)
  target_compile_options(starcraft_core PRIVATE /W4 /permissive- /Zc:preprocessor)
endif()

add_executable(StarCraftRecovery WIN32
  "${IMGUI_ROOT}/imgui.cpp"
  "${IMGUI_ROOT}/imgui_draw.cpp"
  "${IMGUI_ROOT}/imgui_tables.cpp"
  "${IMGUI_ROOT}/imgui_widgets.cpp"
  "${IMGUI_ROOT}/backends/imgui_impl_opengl2.cpp"
  "${IMGUI_ROOT}/backends/imgui_impl_win32.cpp"
  src/platform/bootstrap_probes.cpp
  src/platform/bootstrap_win32.cpp
  src/platform/debug_console.cpp
  src/snp/battle/Account.cpp
  src/snp/battle/Battle.cpp
  src/snp/battle/Chatchnl.cpp
  src/snp/battle/Chatroom.cpp
  src/snp/battle/COnnect.cpp
  src/snp/battle/Joingame.cpp
  src/snp/battle/Logon.cpp
  src/snp/battle/Srv.cpp
  src/snp/battle/Ui.cpp
  src/gds/image.cpp
  src/gds/capture.cpp
  src/gds/sys_Win.cpp
  src/gds/vidblit.cpp
  src/lang/CImage.cpp
  src/lang/CBulletRuntime.cpp
  src/lang/CUnitBuildRuntime.cpp
  src/lang/CUnitInitRuntime.cpp
  src/lang/CUnitPBuildRuntime.cpp
  src/lang/CUnitPSpellsRuntime.cpp
  src/lang/CUnitSpellsRuntime.cpp
  src/lang/CUnitProtossRuntime.cpp
  src/lang/SAI_MainRuntime.cpp
  src/lang/CUnitUpgradeRuntime.cpp
  src/lang/CUnitUpdate.cpp
  src/lang/CUnitZBuildRuntime.cpp
  src/lang/CUnitZergRuntime.cpp
  src/lang/cellscrl.cpp
  src/lang/filedist.cpp
  src/lang/event_Win.cpp
  src/lang/gameloop.cpp
  src/lang/gamedata.cpp
  src/lang/gamesnd.cpp
  src/lang/gluChat.cpp
  src/lang/gluChatSlot.cpp
  src/lang/gluConn.cpp
  src/lang/gluMain.cpp
  src/lang/gluMap.cpp
  src/lang/gluRdy.cpp
  src/lang/gluScore.cpp
  src/lang/glues.cpp
  src/lang/map_load.cpp
  src/lang/minimap.cpp
  src/lang/Music.cpp
  src/lang/placebox.cpp
  src/lang/playflic.cpp
  src/lang/select.cpp
  src/lang/snd.cpp
  src/lang/statmisc.cpp
  src/lang/statcmd.cpp
  src/lang/statdata.cpp
  src/lang/statdraw.cpp
  src/lang/statport.cpp
  src/lang/statres.cpp
  src/lang/statwire.cpp
  src/lang/target.cpp
  src/lang/title.cpp
  src/lang/opt_dlgs.cpp
  src/lang/VictDlg.cpp
  src/lang/victory.cpp
  src/staredit/mapcomn/creep_runtime.cpp
)
target_include_directories(
  StarCraftRecovery
  PRIVATE "${IMGUI_ROOT}" "${IMGUI_ROOT}/backends"
)
target_link_libraries(
  StarCraftRecovery
  PRIVATE starcraft_core vendored_libsmacker OpenAL::OpenAL user32 kernel32 gdi32 opengl32 ws2_32
)
set_target_properties(StarCraftRecovery PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${STARCRAFT_RECOVERY_ROOT}"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${STARCRAFT_RECOVERY_ROOT}"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${STARCRAFT_RECOVERY_ROOT}"
  RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${STARCRAFT_RECOVERY_ROOT}"
  RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${STARCRAFT_RECOVERY_ROOT}"
)
add_custom_command(
  TARGET StarCraftRecovery POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different
          "$<TARGET_FILE:OpenAL>"
          "${STARCRAFT_RECOVERY_ROOT}/$<TARGET_FILE_NAME:OpenAL>"
  COMMENT "Copying the dynamically linked vendored OpenAL Soft runtime"
  VERBATIM
)

add_executable(archive_inventory tools/archive_inventory.cpp)
target_link_libraries(archive_inventory PRIVATE starcraft_core)

add_executable(stat_text_inventory tools/stat_text_inventory.cpp)
target_link_libraries(stat_text_inventory PRIVATE starcraft_core)

add_executable(dialog_inventory tools/dialog_inventory.cpp)
target_link_libraries(dialog_inventory PRIVATE starcraft_core)

add_executable(protoss_data_inventory tools/protoss_data_inventory.cpp)
target_link_libraries(protoss_data_inventory PRIVATE starcraft_core)

if(MSVC)
  # Preserve the original PE's fixed x86 image-base assumption while recovery is
  # address-oriented. This can be relaxed after all absolute-address evidence is gone.
  target_link_options(StarCraftRecovery PRIVATE /BASE:0x00400000 /DYNAMICBASE:NO /FIXED)
endif()

if(BUILD_TESTING)
  if(DOTNET_EXECUTABLE)
    add_test(NAME battle_server_self_test
             COMMAND "${STARCRAFT_RECOVERY_SERVER_EXE}" --self-test)
  endif()

  add_executable(battle_client_probe
    tools/battle_client_probe.cpp
    src/snp/battle/Srv.cpp
    src/snp/battle/Chatchnl.cpp
    src/snp/battle/Chatroom.cpp
    src/snp/battle/Logon.cpp
  )
  target_compile_features(battle_client_probe PRIVATE cxx_std_17)
  target_link_libraries(battle_client_probe PRIVATE ws2_32)

  add_executable(battle_dialog_inventory tools/battle_dialog_inventory.cpp)
  target_compile_features(battle_dialog_inventory PRIVATE cxx_std_17)

  add_executable(battle_art_inventory tools/battle_art_inventory.cpp)
  target_compile_features(battle_art_inventory PRIVATE cxx_std_17)
  target_link_libraries(battle_art_inventory PRIVATE starcraft_core)
  find_program(WINDOWS_POWERSHELL_EXECUTABLE powershell)
  if(DOTNET_EXECUTABLE AND WINDOWS_POWERSHELL_EXECUTABLE)
    add_test(
      NAME battle_client_server_integration
      COMMAND "${WINDOWS_POWERSHELL_EXECUTABLE}"
              -NoProfile -ExecutionPolicy Bypass
              -File "${CMAKE_CURRENT_SOURCE_DIR}/tests/battle_integration_test.ps1"
              -Server "${STARCRAFT_RECOVERY_SERVER_EXE}"
              -Client "$<TARGET_FILE:battle_client_probe>")
    set_tests_properties(battle_client_server_integration PROPERTIES
                         DEPENDS battle_server_self_test TIMEOUT 15)
  endif()
  set(STARCRAFT_INPUT_ROOT "${STARCRAFT_RECOVERY_ROOT}")

  add_executable(damage_tests tests/damage_tests.cpp)
  target_link_libraries(damage_tests PRIVATE starcraft_core)
  add_test(NAME damage_tests COMMAND damage_tests)

  add_executable(cunit_build_tests tests/cunit_build_tests.cpp)
  target_link_libraries(cunit_build_tests PRIVATE starcraft_core)
  add_test(NAME cunit_build_tests COMMAND cunit_build_tests)

  add_executable(cunit_harvest_tests tests/cunit_harvest_tests.cpp)
  target_link_libraries(cunit_harvest_tests PRIVATE starcraft_core)
  add_test(NAME cunit_harvest_tests COMMAND cunit_harvest_tests)

  add_executable(cunit_zerg_tests tests/cunit_zerg_tests.cpp)
  target_link_libraries(cunit_zerg_tests PRIVATE starcraft_core)
  add_test(NAME cunit_zerg_tests COMMAND cunit_zerg_tests)

  add_executable(cunit_terran_tests tests/cunit_terran_tests.cpp)
  target_link_libraries(cunit_terran_tests PRIVATE starcraft_core)
  add_test(NAME cunit_terran_tests COMMAND cunit_terran_tests)

  add_executable(cunit_protoss_tests tests/cunit_protoss_tests.cpp)
  target_link_libraries(cunit_protoss_tests PRIVATE starcraft_core)
  add_test(NAME cunit_protoss_tests COMMAND cunit_protoss_tests)

  add_executable(cunit_path_collide_tests tests/cunit_path_collide_tests.cpp)
  target_link_libraries(cunit_path_collide_tests PRIVATE starcraft_core)
  add_test(NAME cunit_path_collide_tests COMMAND cunit_path_collide_tests)

  add_executable(cunit_init_tests tests/cunit_init_tests.cpp)
  target_link_libraries(cunit_init_tests PRIVATE starcraft_core)
  add_test(NAME cunit_init_tests COMMAND cunit_init_tests)

  add_executable(place_unit_tests tests/place_unit_tests.cpp)
  target_link_libraries(place_unit_tests PRIVATE starcraft_core)
  add_test(NAME place_unit_tests COMMAND place_unit_tests)

  add_executable(tile2_tests tests/tile2_tests.cpp)
  target_link_libraries(tile2_tests PRIVATE starcraft_core)
  add_test(NAME tile2_tests COMMAND tile2_tests)

  add_executable(original_damage_oracle_tests
    tests/original_damage_oracle_tests.cpp
    tests/support/original_image.cpp
  )
  target_include_directories(original_damage_oracle_tests PRIVATE include tests)
  target_compile_features(original_damage_oracle_tests PRIVATE cxx_std_17)
  target_compile_definitions(original_damage_oracle_tests PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
  target_link_libraries(original_damage_oracle_tests PRIVATE starcraft_core kernel32 shell32)
  set_target_properties(original_damage_oracle_tests PROPERTIES WIN32_EXECUTABLE TRUE)
  if(MSVC)
    target_link_options(
      original_damage_oracle_tests
      PRIVATE /BASE:0x10000000 /DYNAMICBASE:NO /FIXED
    )
  endif()
  add_test(
    NAME original_damage_oracle_tests
    COMMAND original_damage_oracle_tests "${STARCRAFT_INPUT_ROOT}"
  )

  add_executable(original_count_oracle_tests
    tests/original_count_oracle_tests.cpp
    tests/support/original_image.cpp
  )
  target_include_directories(original_count_oracle_tests PRIVATE include tests)
  target_compile_features(original_count_oracle_tests PRIVATE cxx_std_17)
  target_compile_definitions(original_count_oracle_tests PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
  target_link_libraries(original_count_oracle_tests PRIVATE starcraft_core kernel32 shell32)
  set_target_properties(original_count_oracle_tests PROPERTIES WIN32_EXECUTABLE TRUE)
  if(MSVC)
    target_link_options(
      original_count_oracle_tests
      PRIVATE /BASE:0x10000000 /DYNAMICBASE:NO /FIXED
    )
  endif()
  add_test(
    NAME original_count_oracle_tests
    COMMAND original_count_oracle_tests "${STARCRAFT_INPUT_ROOT}"
  )

  add_executable(count_tests tests/count_tests.cpp)
  target_link_libraries(count_tests PRIVATE starcraft_core)
  add_test(NAME count_tests COMMAND count_tests)

  add_executable(dat_tests tests/dat_tests.cpp)
  target_link_libraries(dat_tests PRIVATE starcraft_core)
  add_test(NAME dat_tests COMMAND dat_tests)

  add_executable(multiplayer_chk_tests tests/chk_tests.cpp)
  target_link_libraries(multiplayer_chk_tests PRIVATE starcraft_core)
  add_test(NAME multiplayer_chk_tests COMMAND multiplayer_chk_tests)

  add_executable(multiplayer_scenario_tests tests/multiplayer_scenario_tests.cpp)
  target_link_libraries(multiplayer_scenario_tests PRIVATE starcraft_core)
  add_test(
    NAME multiplayer_scenario_tests
    COMMAND multiplayer_scenario_tests "${STARCRAFT_INPUT_ROOT}"
  )

  add_executable(real_map_tests tests/real_map_tests.cpp)
  target_link_libraries(real_map_tests PRIVATE starcraft_core)
  add_test(
    NAME real_map_tests
    COMMAND real_map_tests "${STARCRAFT_INPUT_ROOT}"
  )

  add_executable(flingy_tests tests/flingy_tests.cpp)
  target_link_libraries(flingy_tests PRIVATE starcraft_core)
  add_test(NAME flingy_tests COMMAND flingy_tests)

  add_executable(pathfinding_tests tests/pathfinding_tests.cpp)
  target_link_libraries(pathfinding_tests PRIVATE starcraft_core)
  add_test(NAME pathfinding_tests COMMAND pathfinding_tests)

  add_executable(lockstep_tests tests/lockstep_tests.cpp)
  target_link_libraries(lockstep_tests PRIVATE starcraft_core)
  add_test(NAME lockstep_tests COMMAND lockstep_tests)

  add_executable(original_sync_oracle_tests
    tests/original_sync_oracle_tests.cpp
    tests/support/original_image.cpp
  )
  target_include_directories(original_sync_oracle_tests PRIVATE include tests)
  target_compile_features(original_sync_oracle_tests PRIVATE cxx_std_17)
  target_compile_definitions(original_sync_oracle_tests PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
  target_link_libraries(original_sync_oracle_tests PRIVATE starcraft_core kernel32 shell32)
  set_target_properties(original_sync_oracle_tests PROPERTIES WIN32_EXECUTABLE TRUE)
  if(MSVC)
    target_link_options(
      original_sync_oracle_tests
      PRIVATE /BASE:0x10000000 /DYNAMICBASE:NO /FIXED
    )
  endif()
  add_test(
    NAME original_sync_oracle_tests
    COMMAND original_sync_oracle_tests "${STARCRAFT_INPUT_ROOT}"
  )

  add_executable(iscript_tests tests/iscript_tests.cpp)
  target_link_libraries(iscript_tests PRIVATE starcraft_core)
  add_test(NAME iscript_tests COMMAND iscript_tests)

  add_executable(original_flingy_oracle_tests
    tests/original_flingy_oracle_tests.cpp
    tests/support/original_image.cpp
  )
  target_include_directories(original_flingy_oracle_tests PRIVATE include tests)
  target_compile_features(original_flingy_oracle_tests PRIVATE cxx_std_17)
  target_compile_definitions(original_flingy_oracle_tests PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
  target_link_libraries(original_flingy_oracle_tests PRIVATE starcraft_core kernel32 shell32)
  set_target_properties(original_flingy_oracle_tests PROPERTIES WIN32_EXECUTABLE TRUE)
  if(MSVC)
    target_link_options(
      original_flingy_oracle_tests
      PRIVATE /BASE:0x10000000 /DYNAMICBASE:NO /FIXED
    )
  endif()
  add_test(
    NAME original_flingy_oracle_tests
    COMMAND original_flingy_oracle_tests "${STARCRAFT_INPUT_ROOT}"
  )

  add_executable(core_data_tests tests/core_data_tests.cpp)
  target_link_libraries(core_data_tests PRIVATE starcraft_core)
  add_test(
    NAME core_data_tests
    COMMAND core_data_tests "${STARCRAFT_INPUT_ROOT}"
  )

  add_executable(unit_layout_tests tests/unit_layout_tests.cpp)
  target_include_directories(unit_layout_tests PRIVATE include)
  target_compile_features(unit_layout_tests PRIVATE cxx_std_17)
  add_test(NAME unit_layout_tests COMMAND unit_layout_tests)

  add_executable(image_sprite_layout_tests tests/image_sprite_layout_tests.cpp)
  target_include_directories(image_sprite_layout_tests PRIVATE include)
  target_compile_features(image_sprite_layout_tests PRIVATE cxx_std_17)
  add_test(NAME image_sprite_layout_tests COMMAND image_sprite_layout_tests)

  add_executable(dependency_abi_tests tests/dependency_abi_tests.cpp)
  target_include_directories(dependency_abi_tests PRIVATE include)
  target_compile_features(dependency_abi_tests PRIVATE cxx_std_17)
  target_compile_definitions(dependency_abi_tests PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
  add_test(
    NAME dependency_abi_tests
    COMMAND dependency_abi_tests "${STARCRAFT_INPUT_ROOT}"
  )

  add_executable(meta_bitmap_tests tests/meta_bitmap_tests.cpp)
  target_link_libraries(meta_bitmap_tests PRIVATE starcraft_core)
  add_test(
    NAME meta_bitmap_tests
    COMMAND meta_bitmap_tests "${STARCRAFT_INPUT_ROOT}"
  )

  add_executable(storm_archive_tests tests/storm_archive_tests.cpp)
  target_link_libraries(storm_archive_tests PRIVATE starcraft_core)
  add_test(
    NAME storm_archive_tests
    COMMAND storm_archive_tests "${STARCRAFT_INPUT_ROOT}"
  )

  add_executable(pcx_archive_tests tests/pcx_archive_tests.cpp)
  target_link_libraries(pcx_archive_tests PRIVATE starcraft_core)
  add_test(
    NAME pcx_archive_tests
    COMMAND pcx_archive_tests "${STARCRAFT_INPUT_ROOT}"
  )

  add_executable(hud_asset_tests tests/hud_asset_tests.cpp)
  target_link_libraries(hud_asset_tests PRIVATE starcraft_core)
  add_test(
    NAME hud_asset_tests
    COMMAND hud_asset_tests "${STARCRAFT_INPUT_ROOT}"
  )

  add_test(NAME bootstrap_asset_probe COMMAND StarCraftRecovery --probe-assets)
  add_test(NAME bootstrap_glue_probe COMMAND StarCraftRecovery --probe-glue)
  add_test(NAME bootstrap_battle_ui_probe
           COMMAND StarCraftRecovery --probe-battle-ui)
  add_test(
    NAME bootstrap_game_flow_probe
    COMMAND StarCraftRecovery --probe-game-flow --probe-map-index=0
  )
  add_test(
    NAME bootstrap_glue_zerg_map_probe
    COMMAND StarCraftRecovery --probe-glue --probe-map-index=3 --probe-worker-build-cards
  )
  add_test(
    NAME bootstrap_glue_zerg_race_conversion_probe
    COMMAND StarCraftRecovery --probe-glue --probe-map-index=0 --probe-local-race=0 --probe-worker-build-cards
  )
  add_test(
    NAME bootstrap_glue_zerg_drone_movement_probe
    COMMAND StarCraftRecovery --probe-glue --probe-map-index=0 --probe-local-race=0 --probe-drone-movement
  )
  add_test(
    NAME bootstrap_glue_protoss_race_probe
    COMMAND StarCraftRecovery --probe-glue --probe-map-index=0 --probe-local-race=2 --probe-worker-build-cards
  )
  add_test(
    NAME bootstrap_glue_geyser_probe
    COMMAND StarCraftRecovery --probe-glue --probe-map-index=0 --probe-glue-geyser
  )
  add_test(NAME bootstrap_opengl_probe COMMAND StarCraftRecovery --probe-opengl)
  add_test(NAME bootstrap_selection_probe COMMAND StarCraftRecovery --probe-selection)
  add_test(NAME bootstrap_command_panel_probe COMMAND StarCraftRecovery --probe-command-panel)
  add_test(
    NAME bootstrap_worker_build_cards_probe
    COMMAND StarCraftRecovery --probe-worker-build-cards
  )
  add_test(
    NAME bootstrap_race_building_cards_probe
    COMMAND StarCraftRecovery --probe-race-building-cards
  )
  add_test(
    NAME bootstrap_bunker_probe
    COMMAND StarCraftRecovery --probe-bunker
  )
  add_test(
    NAME bootstrap_terran_air_probe
    COMMAND StarCraftRecovery --probe-terran-air
  )
  add_test(
    NAME bootstrap_protoss_abilities_probe
    COMMAND StarCraftRecovery --probe-protoss-abilities
  )
  add_test(
    NAME bootstrap_race_construction_probe
    COMMAND StarCraftRecovery --probe-race-construction
  )
  add_test(NAME bootstrap_status_panel_probe COMMAND StarCraftRecovery --probe-status-panel)
  add_test(NAME bootstrap_multi_status_probe COMMAND StarCraftRecovery --probe-multi-status)
  add_test(NAME bootstrap_production_probe COMMAND StarCraftRecovery --probe-production)
  add_test(
    NAME bootstrap_all_production_probe
    COMMAND StarCraftRecovery --probe-all-production
  )
  add_test(
    NAME bootstrap_harvest_queue_probe
    COMMAND StarCraftRecovery --probe-harvest-queue
  )
  add_test(NAME bootstrap_geyser_probe COMMAND StarCraftRecovery --probe-geysers)
  add_test(
    NAME bootstrap_building_working_probe
    COMMAND StarCraftRecovery --probe-building-working
  )
  add_test(
    NAME bootstrap_resource_feedback_probe
    COMMAND StarCraftRecovery --probe-resource-feedback
  )
  add_test(
    NAME bootstrap_resource_strip_probe
    COMMAND StarCraftRecovery --probe-resource-strip
  )
  add_test(NAME bootstrap_unit_audio_probe COMMAND StarCraftRecovery --probe-unit-audio)
  add_test(NAME bootstrap_music_probe COMMAND StarCraftRecovery --probe-music)
  add_test(NAME bootstrap_movement_probe COMMAND StarCraftRecovery --probe-movement)
  add_test(NAME bootstrap_pathfinding_probe COMMAND StarCraftRecovery --probe-pathfinding)
  add_test(NAME bootstrap_portrait_probe COMMAND StarCraftRecovery --probe-portrait)
  add_test(NAME bootstrap_worker_actions_probe COMMAND StarCraftRecovery --probe-worker-actions)
  add_test(
    NAME bootstrap_combat_animation_probe
    COMMAND StarCraftRecovery --probe-combat-animation
  )
  add_test(
    NAME bootstrap_projectile_hangar_probe
    COMMAND StarCraftRecovery --probe-projectile-hangar
  )
  add_test(
    NAME bootstrap_harvest_visual_probe
    COMMAND StarCraftRecovery --probe-harvest-visual
  )
  add_test(NAME bootstrap_smart_orders_probe COMMAND StarCraftRecovery --probe-smart-orders)
  add_test(
    NAME bootstrap_refinery_placement_probe
    COMMAND StarCraftRecovery --probe-refinery-placement
  )
  add_test(
    NAME bootstrap_command_target_probe
    COMMAND StarCraftRecovery --probe-command-targets
  )
  add_test(
    NAME bootstrap_building_placement_probe
    COMMAND StarCraftRecovery --probe-building-placement
  )
  add_test(NAME bootstrap_camera_probe COMMAND StarCraftRecovery --probe-camera)
  add_test(NAME bootstrap_minimap_probe COMMAND StarCraftRecovery --probe-minimap)
  add_test(
    NAME bootstrap_unit_avoidance_probe
    COMMAND StarCraftRecovery --probe-unit-avoidance
  )
  add_test(NAME bootstrap_construction_probe COMMAND StarCraftRecovery --probe-construction)
  add_test(
    NAME bootstrap_construction_status_probe
    COMMAND StarCraftRecovery --probe-status-construction
  )
  add_test(NAME bootstrap_addon_probe COMMAND StarCraftRecovery --probe-addon)
  add_test(NAME bootstrap_creep_probe COMMAND StarCraftRecovery --probe-creep)
  add_test(NAME bootstrap_ai_probe COMMAND StarCraftRecovery --probe-ai)
  add_test(
    NAME bootstrap_melee_start_probe
    COMMAND StarCraftRecovery --probe-melee-start
  )

  add_executable(grp_tests tests/grp_tests.cpp)
  target_link_libraries(grp_tests PRIVATE starcraft_core)
  add_test(NAME grp_tests COMMAND grp_tests)

  add_executable(tileset_tests tests/tileset_tests.cpp)
  target_link_libraries(tileset_tests PRIVATE starcraft_core)
  add_test(NAME tileset_tests COMMAND tileset_tests)

  add_executable(tileset_archive_tests tests/tileset_archive_tests.cpp)
  target_link_libraries(tileset_archive_tests PRIVATE starcraft_core)
  add_test(
    NAME tileset_archive_tests
    COMMAND tileset_archive_tests "${STARCRAFT_INPUT_ROOT}"
  )

  add_executable(original_grp_oracle_tests
    tests/original_grp_oracle_tests.cpp
    tests/support/original_image.cpp
  )
  target_include_directories(original_grp_oracle_tests PRIVATE include tests)
  target_compile_features(original_grp_oracle_tests PRIVATE cxx_std_17)
  target_compile_definitions(original_grp_oracle_tests PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
  target_link_libraries(original_grp_oracle_tests PRIVATE starcraft_core kernel32 shell32)
  set_target_properties(original_grp_oracle_tests PROPERTIES WIN32_EXECUTABLE TRUE)
  if(MSVC)
    target_link_options(
      original_grp_oracle_tests
      PRIVATE /BASE:0x10000000 /DYNAMICBASE:NO /FIXED
    )
  endif()
  add_test(
    NAME original_grp_oracle_tests
    COMMAND original_grp_oracle_tests "${STARCRAFT_INPUT_ROOT}"
  )

  add_executable(bitmap_tests tests/bitmap_tests.cpp)
  target_link_libraries(bitmap_tests PRIVATE starcraft_core)
  add_test(NAME bitmap_tests COMMAND bitmap_tests)

  add_executable(original_bitmap_oracle_tests
    tests/original_bitmap_oracle_tests.cpp
    tests/support/original_image.cpp
  )
  target_include_directories(original_bitmap_oracle_tests PRIVATE include tests)
  target_compile_features(original_bitmap_oracle_tests PRIVATE cxx_std_17)
  target_compile_definitions(original_bitmap_oracle_tests PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
  target_link_libraries(original_bitmap_oracle_tests PRIVATE starcraft_core kernel32 shell32)
  set_target_properties(original_bitmap_oracle_tests PROPERTIES WIN32_EXECUTABLE TRUE)
  if(MSVC)
    target_link_options(
      original_bitmap_oracle_tests
      PRIVATE /BASE:0x10000000 /DYNAMICBASE:NO /FIXED
    )
  endif()
  add_test(
    NAME original_bitmap_oracle_tests
    COMMAND original_bitmap_oracle_tests "${STARCRAFT_INPUT_ROOT}"
  )
endif()
