idf_component_register(SRCS 
    "main.c" 
    "./src/camera.c" 
    "./src/wifi.c"
    "./src/http.c"
    "./src/gpio.c"
    "./src/water_sensor.c"
    "./src/temperature.c"
    "./src/sensor_data.c"
                    INCLUDE_DIRS "." "./src")
target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLOG_LOCAL_LEVEL=ESP_LOG_VERBOSE")