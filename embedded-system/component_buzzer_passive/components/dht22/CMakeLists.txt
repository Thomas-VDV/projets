idf_component_register(
    SRCS "buzzer_passive.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES driver esp_timer
)
