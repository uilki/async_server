configure_file(${CMAKE_SOURCE_DIR}/json/requestSchema.json.in ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/requestSchema.json)
configure_file(${CMAKE_SOURCE_DIR}/json/authRequestSchema.json.in ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/authRequestSchema.json)

foreach(ssl_file ${SSL_FILES})
    message("copy ${PROJECT_SOURCE_DIR}/../certs/${ssl_file} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )
    if (EXISTS ${PROJECT_SOURCE_DIR}/../certs/${ssl_file})
        file(COPY ${PROJECT_SOURCE_DIR}/../certs/${ssl_file} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    else()
        message(WARNING "${PROJECT_SOURCE_DIR}/../certs/${ssl_file} -- not found" )
    endif()
endforeach()

