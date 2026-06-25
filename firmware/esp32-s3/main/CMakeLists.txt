# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRCS "main.c" "lcd_st77922.c" "lvgl_ui.c"
    INCLUDE_DIRS "."
)
