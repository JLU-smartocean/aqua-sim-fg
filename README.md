# aqua-sim-fg
We will soon update the full version of aqua-sim-fg, including more network protocols and a more realistic underwater acoustic propagation model. In addition, aqua-sim-fg will also support hybrid programming with MATLAB and Python.

# MATLAB hybrid programming

1. Install MATLAB and MATLAB Compiler
1. Install MATLAB runtime
1. Compile a custom MATLAB function `xxx()` to a C++ library using `mcc -W cpplib:libxxx -T link:lib xxx`
1. Move `libxxx.h` to `NS3PATH/src/aqua-sim-fg/model/include` and Move `libxxx.so` to `NS3PATH/src/aqua-sim-fg/model/lib`
1. Append MATLAB-related header and library files paths to `NS3PATH/CMakeLists.txt` as follow:
```cmake
include_directories(MATLAB_RUNTIME_PATH/R2023a/extern/include)
link_directories(MATLAB_RUNTIME_PATH/R2023a/runtime/glnxa64)
link_directories(${CMAKE_SOURCE_DIR}/src/aqua-sim-fg/model/lib)
link_libraries(mwmclmcrrt)
link_libraries(xxx)
```
You may need to append the MATLAB Runtime library to LD_LIBRARY_PATH if it's not an Ubuntu 22 system.
6.  Enable MATLAB using `asHelper.SetMatlab(true)`

7. An example is as follows:

   ![image-20241203143109727](/home/ch/.config/Typora/typora-user-images/image-20241203143109727.png)


# References
[1] Robert Martin, Sanguthevar Rajasekaran, and Zheng Peng. 2017. Aqua-Sim Next Generation: An NS-3 Based Underwater Sensor Network Simulator. In Proceedings of the 12th International Conference on Underwater Networks & Systems (WUWNet '17). Association for Computing Machinery, New York, NY, USA, Article 3, 1–8. https://doi.org/10.1145/3148675.3148679

[2] Jiani Guo, Jun Liu, Shanshan Song, Chuang Zhang, Hao Chen, and JunHong Cui. 2021. Aqua-Psim: A Semi-Physical Simulation Platform Based on NS3 for Underwater Acoustic Network. In The 15th International Conference on Underwater Networks & Systems (WUWNet’21), November 22–24, 2021, Shenzhen, Guangdong, China. ACM, New York, NY, USA, 5 pages.
