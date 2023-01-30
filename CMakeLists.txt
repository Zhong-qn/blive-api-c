set(CMAKE_LEGACY_CYGWIN_WIN32 0)
cmake_minimum_required(VERSION 3.7)

# gcc conn.c ../external_supports/cJSON/cJSON.c -I. -I../external_supports/ -I../external_supports/curl/include -I../include/blive_api -lcurl
project(blive_api_c C)

set(MAJOR_VERSION  0)
set(MINOR_VERSION  0)
set(PATCH_VERSION  1)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_C_COMPILER gcc)

# 创建项目工作路径
if(DEFINED BLIVE_API_DIR)
message("already difined work directory: ${BLIVE_API_DIR}")
else()
message("not difined work directory, using default:  ${CMAKE_CURRENT_SOURCE_DIR}")
set(BLIVE_API_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# 创建编译路径
if(DEFINED BLIVE_API_BUILD_DIR)
message("already difined build directory: ${BLIVE_API_BUILD_DIR}")
else()
message("not difined build directory, using default:  ${BLIVE_API_DIR}/build/")
set(BLIVE_API_BUILD_DIR ${BLIVE_API_DIR}/build)
endif()

# 创建lib输出路径
if(DEFINED LIB_OUTPUT_DIR)
message("already difined libarary output directory: ${LIB_OUTPUT_DIR}")
else()
message("not difined libarary output directory, using default:  ${BLIVE_API_BUILD_DIR}/lib/")
set(LIB_OUTPUT_DIR ${BLIVE_API_BUILD_DIR}/output/lib)
endif()
set(LIBRARY_OUTPUT_PATH ${LIB_OUTPUT_DIR})

# 创建头文件输出路径
if(DEFINED INCLUDE_OUTPUT_DIR)
message("already difined include file output directory: ${INCLUDE_OUTPUT_DIR}")
else()
message("not difined include file output directory, using default:  ${BLIVE_API_BUILD_DIR}/include/")
set(INCLUDE_OUTPUT_DIR ${BLIVE_API_BUILD_DIR}/output/include)
endif()

# 创建cmake临时文件路径
if(DEFINED TMP_FILES_DIR)
message("already difined cache file directory: ${TMP_FILES_DIR}")
else()
message("not difined cache file directory, using default:  ${BLIVE_API_DIR}/CMakeCaches/")
set(TMP_FILES_DIR ${BLIVE_API_DIR}/CMakeCaches)
endif()


include_directories(${BLIVE_API_DIR}/include
                    ${BLIVE_API_DIR}/include/blive_api
                    ${BLIVE_API_DIR}/source
                    ${BLIVE_API_DIR}/external_supports/
                    ${BLIVE_API_DIR}/external_supports/curl/include
                    ${BLIVE_API_DIR}/external_supports/brotli/include)

link_directories(./build/output/lib/)


set(EXT_CJSON_SRC       ${BLIVE_API_DIR}/external_supports/cJSON/cJSON.c
                        ${BLIVE_API_DIR}/external_supports/cJSON/cJSON_Utils.c
                        )

set(EXT_BROTLI_SRC      ${BLIVE_API_DIR}/external_supports/brotli/dec/bit_reader.c
                        ${BLIVE_API_DIR}/external_supports/brotli/dec/decode.c
                        ${BLIVE_API_DIR}/external_supports/brotli/dec/huffman.c
                        ${BLIVE_API_DIR}/external_supports/brotli/dec/state.c
                        ${BLIVE_API_DIR}/external_supports/brotli/common/constants.c
                        ${BLIVE_API_DIR}/external_supports/brotli/common/context.c
                        ${BLIVE_API_DIR}/external_supports/brotli/common/dictionary.c
                        ${BLIVE_API_DIR}/external_supports/brotli/common/platform.c
                        ${BLIVE_API_DIR}/external_supports/brotli/common/shared_dictionary.c
                        ${BLIVE_API_DIR}/external_supports/brotli/common/transform.c
                        )

set(BLIVE_API_SRC       ${BLIVE_API_DIR}/source/conn.c
                        ${BLIVE_API_DIR}/source/msg.c
                        ${BLIVE_API_DIR}/source/blive.c
                        ${BLIVE_API_DIR}/source/log.c
                        )


add_compile_options(-g -Wall -fPIC)


add_library(blive_api SHARED ${BLIVE_API_SRC})
add_library(blive_api_s STATIC ${BLIVE_API_SRC})

target_link_libraries(blive_api pthread curl brotli_s cjson_s)
target_link_libraries(blive_api_s pthread curl brotli_s cjson_s)
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(blive_api ws2_32)
    target_link_libraries(blive_api_s ws2_32)
endif()

file(COPY ${BLIVE_API_DIR}/external_supports/cJSON/cJSON.h      DESTINATION ${INCLUDE_OUTPUT_DIR})
file(COPY ${BLIVE_API_DIR}/include/blive_api/blive_api.h        DESTINATION ${INCLUDE_OUTPUT_DIR}/blive_api/)
file(COPY ${BLIVE_API_DIR}/include/blive_api/blive_def.h        DESTINATION ${INCLUDE_OUTPUT_DIR}/blive_api/)

add_library(brotli SHARED ${EXT_BROTLI_SRC})
add_library(brotli_s STATIC ${EXT_BROTLI_SRC})

add_library(cjson SHARED ${EXT_CJSON_SRC})
add_library(cjson_s STATIC ${EXT_CJSON_SRC})
