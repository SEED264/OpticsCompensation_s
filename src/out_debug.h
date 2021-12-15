#ifndef _OPTICSCOMPENSATION_S_SRC_OUT_DEBUG_H_
#define _OPTICSCOMPENSATION_S_SRC_OUT_DEBUG_H_

#include <string>
#include <aut/AUL_UtilFunc.h>


#ifdef _DEBUG

template<typename... T>
void OutDebugInfo(T... out_info) {
    aut::DebugPrint(out_info);
}

#else

template<typename... T>
void OutDebugInfo(T... out_info) {}

#endif

#endif // _OPTICSCOMPENSATION_S_SRC_OUT_DEBUG_H_