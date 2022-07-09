#查库
find_package (Libevent CONFIG REQUIRED)
find_package (OpenSSL REQUIRED)

#设置目标名称
set (TARGET_NAME ${SERVER_TARGET})

#添加源文件
aux_source_directory (${CMAKE_CURRENT_SOURCE_DIR} TARGET_SRC)
aux_source_directory (${LOGC_DIR} TARGET_SRC)
set (TARGET_SRC ${TARGET_SRC} ${OPENSSL_APPLINK_SOURCE})

#添加包含目录
include_directories (${OPENSSL_INCLUDE_DIR})
include_directories (${LOGC_DIR})

#设置编译宏
add_compile_definitions (LOG_USE_COLOR)
if (CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	add_compile_definitions (_DEBUG)
elseif (CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
	add_compile_definitions (NDEBUG)
endif ()
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_compile_definitions (_CRT_SECURE_NO_WARNINGS)
endif ()

#设置编译器参数
if (CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		add_compile_options (
			/GS#安全检查:启用安全检查
			/Zc:wchar_t#将WChar_t视为内置类型:是
			/Gm-#启用最小重新生成:否
			/sdl#SDL检查:是
			/fp:precise#浮点模型:精度
			/Zc:forScope#强制For循环范围中的合规性:是
			/GR#启用运行时类型信息:是
			/Gd#调用约定:__cdecl
			/nologo#取消显示启动版权标志:是
			/bigobj#较大生成对象:是
		)
		add_link_options (
			/MANIFEST#生成清单:是
			/NXCOMPAT#数据执行保护(DEP):是
			/DYNAMICBASE#随机基址:是
			/LARGEADDRESSAWARE#启用大地址:是
			/SUBSYSTEM:CONSOLE#子系统:控制台
			/MANIFESTUAC:NO#启用用户账户控制:否
			/NOLOGO#取消显示启动版权标志:是
		)
	else ()
		add_compile_options (-fPIE)
		add_link_options (-pie)
	endif ()
elseif (CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
	if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		add_compile_options (
			/GS#安全检查:启用安全检查
			/GL#全程序优化:是
			/Gy#启用函数级链接:是
			/Zc:wchar_t#将WChar_t视为内置类型:是
			/Gm-#启用最小重新生成:否
			/sdl#SDL检查:是
			/fp:precise#浮点模型:精度
			/Zc:forScope#强制For循环范围中的合规性:是
			/GR#启用运行时类型信息:是
			/Gd#调用约定:__cdecl
			/nologo#取消显示启动版权标志:是
			/bigobj#较大生成对象:是
		)
		add_link_options (
			/MANIFEST#生成清单:是
			/LTCG#链接时间代码生成:使用链接时间代码生成
			/NXCOMPAT#数据执行保护(DEP):是
			/DYNAMICBASE#随机基址:是
			/LARGEADDRESSAWARE#启用大地址:是
			/OPT:REF#引用:是
			/SUBSYSTEM:CONSOLE#子系统:控制台
			/MANIFESTUAC:NO#启用用户账户控制:否
			/OPT:ICF#启用COMDAT折叠:是
			/NOLOGO#取消显示启动版权标志:是
		)
	else ()
		add_compile_options (-fPIE)
		add_link_options (-pie)
	endif ()
endif ()

#设置编译目标
add_executable (${TARGET_NAME} ${TARGET_SRC})

#设置openssl链接
set (OPENSSL_LIB OpenSSL::SSL OpenSSL::Crypto)

#libevent配置
set (LIBEVENT_MODULE libevent::core libevent::extra libevent::openssl)
if (NOT WIN32)
	set (LIBEVENT_MODULE ${LIBEVENT_MODULE} libevent::pthreads)
	set (LIB_MATH m)
endif ()

#链接库
target_link_libraries (${TARGET_NAME} PRIVATE ${LIBEVENT_MODULE} ${OPENSSL_LIB} ${LIB_MATH})

#设置目标位置
set_target_properties (
	${TARGET_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${OUT_DIR}/${TARGET_NAME}"
    LIBRARY_OUTPUT_DIRECTORY "${OUT_DIR}/${TARGET_NAME}"
    RUNTIME_OUTPUT_DIRECTORY "${OUT_DIR}/${TARGET_NAME}"
)

#设置目标依赖关系
add_dependencies (${TARGET_NAME} ${DICTC_TARGET})

#运行环境配置
if (WIN32)
	file (GLOB ETC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/etc/*)
	foreach (ITEM ${ETC_FILE})
		add_custom_command (
			TARGET ${TARGET_NAME}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E
			copy_if_different
			${ITEM} ${OUT_DIR}/${TARGET_NAME}/
		)
	endforeach ()
else ()
	add_custom_command (
		TARGET ${TARGET_NAME}
		POST_BUILD
		COMMAND mkdir -p
		${OUT_DIR}/pack/
	)
	add_custom_command (
		TARGET ${TARGET_NAME}
		POST_BUILD
		COMMAND cp -r -u
		${PACK_DIR}/* ${OUT_DIR}/pack/
	)

	add_custom_command (
		TARGET ${TARGET_NAME}
		POST_BUILD
		COMMAND mkdir -p
		"${OUT_DIR}/pack/tmp/dict-on-ray/"
	)
	add_custom_command (
		TARGET ${TARGET_NAME}
		POST_BUILD
		COMMAND cp -u
		"${CMAKE_CURRENT_SOURCE_DIR}/etc/ENG_ZH.dicx" "${OUT_DIR}/pack/tmp/dict-on-ray/"
	)
	add_custom_command (
		TARGET ${TARGET_NAME}
		POST_BUILD
		COMMAND mkdir -p
		"${OUT_DIR}/pack/usr/bin/"
	)
	add_custom_command (
		TARGET ${TARGET_NAME}
		POST_BUILD
		COMMAND cp -u
		$<TARGET_FILE:${TARGET_NAME}> "${OUT_DIR}/pack/usr/bin/dict-server"
	)

	add_custom_command (
		TARGET ${TARGET_NAME}
		POST_BUILD
		COMMAND chmod 755
		"${OUT_DIR}/pack" -R
	)
	add_custom_command (
		TARGET ${TARGET_NAME}
		POST_BUILD
		COMMAND dpkg -b
		"${OUT_DIR}/pack" "${OUT_DIR}/Dict-on-ray-server-ubuntu2004-amd64.deb"
	)
endif ()
