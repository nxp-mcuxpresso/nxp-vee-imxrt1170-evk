include_guard()
message("microej/net component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/src/async_select_cache.c
	${CMAKE_CURRENT_LIST_DIR}/src/async_select.c
	${CMAKE_CURRENT_LIST_DIR}/src/async_select_osal.c
	${CMAKE_CURRENT_LIST_DIR}/src/LLNET_CHANNEL_bsd.c
	${CMAKE_CURRENT_LIST_DIR}/src/LLNET_Common.c
	${CMAKE_CURRENT_LIST_DIR}/src/LLNET_DNS_native_impl.c
	${CMAKE_CURRENT_LIST_DIR}/src/LLNET_DATAGRAMSOCKETCHANNEL_bsd.c
	${CMAKE_CURRENT_LIST_DIR}/src/LLNET_NETWORKADDRESS_bsd.c
	${CMAKE_CURRENT_LIST_DIR}/src/LLNET_SOCKETCHANNEL_bsd.c
	${CMAKE_CURRENT_LIST_DIR}/src/LLNET_NETWORKINTERFACE_lwip.c
	${CMAKE_CURRENT_LIST_DIR}/src/LLNET_STREAMSOCKETCHANNEL_bsd.c
	${CMAKE_CURRENT_LIST_DIR}/src/lwip_util.c
	${CMAKE_CURRENT_LIST_DIR}/src/mac_address_util.c
	${CMAKE_CURRENT_LIST_DIR}/src/LLNET_MULTICASTSOCKETCHANNEL_bsd.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc)
target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc/sys)
target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc/arpa)
target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc/netinet)