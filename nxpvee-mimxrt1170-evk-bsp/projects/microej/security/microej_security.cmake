include_guard()
message("microej/security component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/src/LLSEC_RANDOM_impl.c
	${CMAKE_CURRENT_LIST_DIR}/src/LLSEC_X509_CERT_impl.c
)
target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc)