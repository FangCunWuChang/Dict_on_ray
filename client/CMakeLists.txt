#设置目标名称
set (TARGET_NAME "client")

#添加源文件
if (APPLE)
	file (GLOB TARGET_SRC ${CMAKE_CURRENT_SOURCE_DIR}/JuceHeader/*.mm)
else ()
	file (GLOB TARGET_SRC ${CMAKE_CURRENT_SOURCE_DIR}/JuceHeader/*.cpp)
endif ()
aux_source_directory (${CMAKE_CURRENT_SOURCE_DIR} TARGET_SRC)

#添加包含目录
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/JuceHeader)
include_directories (${JUCE_DIR})

#设置编译宏
if (CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	add_compile_definitions (_DEBUG)
elseif (CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
	add_compile_definitions (NDEBUG)
endif ()
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_compile_definitions (_CRT_SECURE_NO_WARNINGS)
endif ()
add_compile_definitions (
	JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_USE_DARK_SPLASH_SCREEN=1
    JUCE_MODULE_AVAILABLE_juce_analytics=1
    JUCE_MODULE_AVAILABLE_juce_audio_basics=0
    JUCE_MODULE_AVAILABLE_juce_audio_devices=0
    JUCE_MODULE_AVAILABLE_juce_audio_formats=0
    JUCE_MODULE_AVAILABLE_juce_audio_processors=0
    JUCE_MODULE_AVAILABLE_juce_audio_utils=0
    JUCE_MODULE_AVAILABLE_juce_box2d=0
    JUCE_MODULE_AVAILABLE_juce_core=1
    JUCE_MODULE_AVAILABLE_juce_cryptography=1
    JUCE_MODULE_AVAILABLE_juce_data_structures=1
    JUCE_MODULE_AVAILABLE_juce_dsp=0
    JUCE_MODULE_AVAILABLE_juce_events=1
    JUCE_MODULE_AVAILABLE_juce_graphics=1
    JUCE_MODULE_AVAILABLE_juce_gui_basics=1
    JUCE_MODULE_AVAILABLE_juce_gui_extra=1
    JUCE_MODULE_AVAILABLE_juce_opengl=1
    JUCE_MODULE_AVAILABLE_juce_osc=0
    JUCE_MODULE_AVAILABLE_juce_product_unlocking=0
    JUCE_MODULE_AVAILABLE_juce_video=0
    JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    JUCE_STANDALONE_APPLICATION=0
    JucePlugin_Build_VST=0
    JucePlugin_Build_VST3=0
    JucePlugin_Build_AU=0
    JucePlugin_Build_AUv3=0
    JucePlugin_Build_RTAS=0
    JucePlugin_Build_AAX=0
    JucePlugin_Build_Standalone=0
    JucePlugin_Build_Unity=0
)
add_compile_definitions (JUCE_MODAL_LOOPS_PERMITTED=1)

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
			/SUBSYSTEM:WINDOWS#子系统:窗口
			/MANIFESTUAC:NO#启用用户账户控制:否
			/NOLOGO#取消显示启动版权标志:是
		)
	else ()
		add_compile_options (-fPIE -fvisibility=hidden)
		add_link_options (-pie -fvisibility=hidden)
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
			/SUBSYSTEM:WINDOWS#子系统:窗口
			/MANIFESTUAC:NO#启用用户账户控制:否
			/OPT:ICF#启用COMDAT折叠:是
			/NOLOGO#取消显示启动版权标志:是
		)
	else ()
		add_compile_options (-fPIE -fvisibility=hidden)
		add_link_options (-pie -fvisibility=hidden)
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