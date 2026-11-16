#ifndef ROBOCOPY_TOOL_CONFIG_CONFIG_H
#define ROBOCOPY_TOOL_CONFIG_CONFIG_H

// C23 add many new adventures as bool ,inline, enum : T ,static_assert ,nullptr
/***************************************************************
 **                       ALLOCER                             **
 ***************************************************************/
#include <assert.h>
#include <stdint.h>
typedef unsigned long long buffer_t;
#define BUFFER_T_SIZE sizeof(buffer_t)
/**
 * @note 默认8个块,16个字节，处理元数据:总的可用缓冲区大小,总共的快大小
 */
#define DEFAULT_BUF_SIZE 9232ULL

#define DEFAULT_BLOCK_SIZE 1152ULL
#define ERR_BLOCK_ID       0xFFFFFFFFFFFFFFFF
#define PTR_T_SIZE         sizeof(uintptr_t)

typedef enum _Allocer_Err Allocer_Err;
/***************************************************************
 **                     ALLOCER CHECK                         **
 ***************************************************************/
static_assert(BUFFER_T_SIZE == 8ULL);
static_assert(DEFAULT_BLOCK_SIZE <= (DEFAULT_BUF_SIZE - 16ULL));
#endif // !ROBOCOPY_TOOL_CONFIG_CONFIG_H