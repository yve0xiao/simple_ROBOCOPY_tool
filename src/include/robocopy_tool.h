#ifndef ROBOCOPY_TOOL_SRC_INCLUDE_ROBOCOPY_TOOL_H
#define ROBOCOPY_TOOL_SRC_INCLUDE_ROBOCOPY_TOOL_H
#warning "This header is deprecated. "
#include "config/config.h"
typedef struct _Path_Data
{
    char m_source_path[DEFAULT_BLOCK_SIZE];
    char m_diraction_path[DEFAULT_BLOCK_SIZE];
} Path_Data;
#endif // !ROBOCOPY_TOOL_SRC_INCLUDE_ROBOCOPY_TOOL_H