//
// MATLAB Compiler: 8.6 (R2023a)
// Date: Mon Dec  2 16:45:47 2024
// Arguments: "-B""macro_default""-W""cpplib:libtest""-T""link:lib""test"
//

#ifndef libtest_h
#define libtest_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#include "mclcppclass.h"
#ifdef __cplusplus
extern "C" { // sbcheck:ok:extern_c
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_libtest_C_API 
#define LIB_libtest_C_API /* No special import/export declaration */
#endif

/* GENERAL LIBRARY FUNCTIONS -- START */

extern LIB_libtest_C_API 
bool MW_CALL_CONV libtestInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_libtest_C_API 
bool MW_CALL_CONV libtestInitialize(void);
extern LIB_libtest_C_API 
void MW_CALL_CONV libtestTerminate(void);

extern LIB_libtest_C_API 
void MW_CALL_CONV libtestPrintStackTrace(void);

/* GENERAL LIBRARY FUNCTIONS -- END */

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

extern LIB_libtest_C_API 
bool MW_CALL_CONV mlxTest(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */

#ifdef __cplusplus
}
#endif


/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__MINGW64__)

#ifdef EXPORTING_libtest
#define PUBLIC_libtest_CPP_API __declspec(dllexport)
#else
#define PUBLIC_libtest_CPP_API __declspec(dllimport)
#endif

#define LIB_libtest_CPP_API PUBLIC_libtest_CPP_API

#else

#if !defined(LIB_libtest_CPP_API)
#if defined(LIB_libtest_C_API)
#define LIB_libtest_CPP_API LIB_libtest_C_API
#else
#define LIB_libtest_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_libtest_CPP_API void MW_CALL_CONV test(int nargout, mwArray& ans, const mwArray& a, const mwArray& b);

/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */
#endif

#endif
