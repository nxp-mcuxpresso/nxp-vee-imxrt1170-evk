include_guard()
message("microej/ssl component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/src/LLNET_SSL_CONTEXT_impl.c
	${CMAKE_CURRENT_LIST_DIR}/src/LLNET_SSL_ERRORS.c
	${CMAKE_CURRENT_LIST_DIR}/src/LLNET_SSL_SOCKET_impl.c
	${CMAKE_CURRENT_LIST_DIR}/src/LLNET_SSL_utils_mbedtls.c
	${CMAKE_CURRENT_LIST_DIR}/src/LLNET_SSL_verifyCallback.c
)
target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc)