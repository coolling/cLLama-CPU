set(TARGET llama-cli)

add_executable(${TARGET} main.cpp)
# 添加 Python 头文件目录
include_directories(/usr/include/python3.10)

# 添加 Python 库目录
link_directories(/usr/lib/python3.10/config-3.10-x86_64-linux-gnu/)
install(TARGETS ${TARGET} RUNTIME)
target_link_libraries(${TARGET} PRIVATE common llama ${CMAKE_THREAD_LIBS_INIT} python3.10)
target_compile_features(${TARGET} PRIVATE cxx_std_11)
