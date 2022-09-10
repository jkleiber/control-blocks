
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
#include <nfd.h>
#ifdef __cplusplus
}
#endif

#include <stdexcept>
#include <string>

bool OpenFileDialog(std::string *path);

bool SaveFileDialog(std::string *path);
