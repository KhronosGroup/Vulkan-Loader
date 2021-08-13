

# Uncomment to rebuild the targets
if(UNIX)
#     add_library(dummy_library_elf_64 SHARED stub.cpp)
#     set_target_properties(dummy_library_elf_64 PROPERTIES COMPILE_OPTIONS "-m64" LINK_FLAGS "-m64")
#     add_custom_command(TARGET dummy_library_elf_64 POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dummy_library_elf_64> ${CMAKE_CURRENT_SOURCE_DIR}/binaries)

#     add_library(dummy_library_elf_32 SHARED stub.cpp)
#     set_target_properties(dummy_library_elf_32 PROPERTIES COMPILE_OPTIONS "-m32" LINK_FLAGS "-m32")
#     add_custom_command(TARGET dummy_library_elf_32 POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dummy_library_elf_32> ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
endif()

if(WIN32)
    # add_library(dummy_library_pe_64 SHARED stub.cpp)
    # add_custom_command(TARGET dummy_library_pe_64 POST_BUILD
    #     COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dummy_library_pe_64> ${CMAKE_CURRENT_SOURCE_DIR}/binaries)

    # add_library(dummy_library_pe_32 SHARED stub.cpp)
    # add_custom_command(TARGET dummy_library_pe_32 POST_BUILD
    #     COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dummy_library_pe_32> ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
endif()
