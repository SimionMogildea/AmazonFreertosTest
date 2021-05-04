cmake_minimum_required(VERSION 3.13)

project(freertos_examples)

# Tell IDF build to link against this target.
set(IDF_PROJECT_EXECUTABLE my_beautiful_app)
set(IDF_SDKCONFIG_DEFAULTS "${CMAKE_CURRENT_LIST_DIR}/sdkconfig")
set(esp_idf_dir "${CMAKE_CURRENT_LIST_DIR}/amazon-freertos/vendors/espressif/esp-idf")

get_filename_component(
    IDF_EXECUTABLE_SRCS
    "main.c" ABSOLUTE
    )

# Add FreeRTOS as a subdirectory. AFR_BOARD tells which board to target.
set(AFR_BOARD espressif.esp32_devkitc CACHE INTERNAL "")
add_subdirectory(amazon-freertos)

# Link against the mqtt library so that we can use it. Dependencies are transitively
# linked.
target_link_libraries(my_beautiful_app 
	PRIVATE 
	AFR::mqtt
	idf::esp_http_server
    idf::esp_http_client
    idf::esp_https_ota
)