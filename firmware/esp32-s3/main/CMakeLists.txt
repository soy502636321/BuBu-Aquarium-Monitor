# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRCS "main.cpp" 
    INCLUDE_DIRS "."
)

target_compile_definitions(${COMPONENT_LIB}
    PUBLIC LV_CONF_INCLUDE_SIMPLE=1
)