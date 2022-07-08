#设置目标名称
set (TARGET_NAME ${DIICTC_TARGET})

#添加源文件
aux_source_directory (${CMAKE_CURRENT_SOURCE_DIR} TARGET_SRC)

#设置编译宏
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

#设置目标位置
set_target_properties (
	${TARGET_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${OUT_DIR}/${TARGET_NAME}"
    LIBRARY_OUTPUT_DIRECTORY "${OUT_DIR}/${TARGET_NAME}"
    RUNTIME_OUTPUT_DIRECTORY "${OUT_DIR}/${TARGET_NAME}"
)

#运行环境配置
add_custom_command (
	TARGET ${TARGET_NAME}
	POST_BUILD
	COMMAND $<TARGET_FILE:${TARGET_NAME}>
	--in ${ECDICT_DIR}/ecdict.csv
	--out ${SERVER_SOURCE}/etc/ENG_ZH.dicx
)
if (NOT WIN32)
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
		"${SERVER_SOURCE}/etc/ENG_ZH.dicx" "${OUT_DIR}/pack/tmp/dict-on-ray/"
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
		$<TARGET_FILE:${TARGET_NAME}> "${OUT_DIR}/pack/usr/bin/dictc"
	)
endif ()
