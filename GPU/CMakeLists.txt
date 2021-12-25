cmake_minimum_required(VERSION 3.6)
project(RayTrace)
# Enable C++20
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_COMPILER "/usr/local/bin/gcc-11") #这里写你的gcc路径
set(CMAKE_CXX_COMPILER "/usr/bin/g++") #这里写你的g++路径
set(OPENMP_LIBRARIES "/usr/local/cellar/libomp/13.0.0/lib") #这里写你的libomp路径, 通过brew安装的默认地址
set(OPENMP_INCLUDES "/usr/local/cellar/libomp/13.0.0/include")#这里写你的libomp路径

message("here")
message("${CMAKE_CXX_COMPILER}")

OPTION (USE_OpenMP "Use OpenMP to enamble <omp.h>" ON)

# Find OpenMP
if(APPLE AND USE_OpenMP)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(OpenMP_C "${CMAKE_C_COMPILER}")
        set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -lomp -Wno-unused-command-line-argument")
        #注意以上需要增加-Xpreprocessor 以及不能直接-llibomp 在这里不需要前缀lib只需要-lomp即可，下面相似的地方也是同个道理
        set(OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5")
        set(OpenMP_libomp_LIBRARY ${OpenMP_C_LIB_NAMES})
        set(OpenMP_libgomp_LIBRARY ${OpenMP_C_LIB_NAMES})
        set(OpenMP_libiomp5_LIBRARY ${OpenMP_C_LIB_NAMES})
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
        set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -lomp -Wno-unused-command-line-argument")
        set(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
        set(OpenMP_libomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
        set(OpenMP_libgomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
        set(OpenMP_libiomp5_LIBRARY ${OpenMP_CXX_LIB_NAMES})
    endif()
endif()

if(USE_OpenMP)
    find_package(OpenMP REQUIRED)
endif(USE_OpenMP)


if (OPENMP_FOUND)
    include_directories("${OPENMP_INCLUDES}")
    link_directories("${OPENMP_LIBRARIES}")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -lncurses -isystem /usr/local/include")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -lncurses -isystem /usr/local/include")
endif(OPENMP_FOUND)

# glfw和glew的头文件目录，这是我的路径，你可以换成你自己的
include_directories(/usr/local/include)
aux_source_directory("./" SRCS)

# glew的库目录路径
link_directories(/usr/local/Cellar/glew/2.2.0_1/lib)
# glfw的库目录路径
link_directories(/usr/local/Cellar/glfw/3.3.6/lib)
add_executable(RayTrace ${SRCS} ${HDRS})

# 指定glew库
target_link_libraries(RayTrace GLEW)
# 指定glfw库
target_link_libraries(RayTrace GLFW)
