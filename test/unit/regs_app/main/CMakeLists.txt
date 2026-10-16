set(repo_main ${CMAKE_CURRENT_LIST_DIR}/../../../../main)
idf_component_register(
    SRCS "test_main.c" "test_regs_description.c" "test_support.c" "${repo_main}/regs_description.c"
    INCLUDE_DIRS "../stubs" "${repo_main}"
    REQUIRES unity
    WHOLE_ARCHIVE
)
target_compile_definitions(${COMPONENT_LIB} PRIVATE CONFIG_LWIP_TCP_OVERSIZE_MSS=1 xQueueHandle=QueueHandle_t)
