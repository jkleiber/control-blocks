#include "controlblocks/file_utils.h"

bool OpenFileDialog(std::string *path)
{
    nfdchar_t *out_path = NULL;
    nfdresult_t result = NFD_OpenDialog(NULL, NULL, &out_path);

    if (result == NFD_OKAY)
    {
        std::string filepath(out_path);
        *path = filepath;
        // TODO: check to see if this causes a memory leak.
        return true;
    }
    else if (result == NFD_CANCEL)
    {
    }
    else
    {
        std::string err = NFD_GetError();
        std::string err_msg = "Error: " + err;
        throw std::runtime_error(err_msg);
    }

    return false;
}

bool SaveFileDialog(std::string *path)
{
    nfdchar_t *out_path = NULL;
    nfdresult_t result = NFD_SaveDialog(NULL, NULL, &out_path);

    if (result == NFD_OKAY)
    {
        std::string filepath(out_path);
        *path = filepath;
        // TODO: check to see if this causes a memory leak.
        return true;
    }
    else if (result == NFD_CANCEL)
    {
    }
    else
    {
        std::string err = NFD_GetError();
        std::string err_msg = "Error: " + err;
        throw std::runtime_error(err_msg);
    }

    return false;
}