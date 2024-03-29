#include <exception>
#include <string>
#include <CL/cl.hpp>
#include "cl_error_code.h"

class InitOpenCLManagerException : public std::exception {
public:
    InitOpenCLManagerException(const std::string &where, cl_uint code)
        : where_(where),
          code_(code) {}

    std::string message() const {
        return (std::string("Failed init OpenCLManager at ") + where_ + " : " +
               cl_error_code[code_]).c_str();
    }

private:
    std::string where_;
    cl_uint code_;
};

inline void CheckCLErrorCode(const std::string &where, cl_int code) {
    if (code == CL_SUCCESS)
        return;

    throw InitOpenCLManagerException(where, code);
}