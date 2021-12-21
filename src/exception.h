#include <exception>
#include <string>
#include <CL/cl.hpp>
#include "CL_ErrorCode.h"

class InitOpenCLManagerException : public std::exception {
public:
    InitOpenCLManagerException(const std::string &where, cl_uint code) {}

    std::string message() const {
        return (std::string("Failed init OpenCLManager at ") + where + " : " + cl_error_code[code]).c_str();
    }

private:
    std::string where;
    cl_uint code;
};

inline void CheckCLErrorCode(const std::string &where, cl_int code) {
    if (code == CL_SUCCESS)
        return;

    throw InitOpenCLManagerException(where, code);
}