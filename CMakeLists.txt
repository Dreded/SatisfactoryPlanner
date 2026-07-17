cmake_minimum_required (VERSION 3.10)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("SatisfactoryPlanner")

add_executable(
    SatisfactoryPlanner

    "src/main.cpp"
    "src/Core/GameDatabase.cpp"
 "src/Core/ProductionPlanner.cpp" "src/Core/FactoryPrinter.cpp")

add_custom_command(
    TARGET SatisfactoryPlanner POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/data"
    "$<TARGET_FILE_DIR:SatisfactoryPlanner>/data"
)

target_include_directories(
    SatisfactoryPlanner
    PRIVATE
    "include"
)

find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(
    SatisfactoryPlanner
    PRIVATE
    nlohmann_json::nlohmann_json
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SatisfactoryPlanner PROPERTY CXX_STANDARD 20)
endif()