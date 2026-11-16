#ifndef ROBOCOPY_TOOL_ALLOCER_ALLOCER_BASE_H
#define ROBOCOPY_TOOL_ALLOCER_ALLOCER_BASE_H
#include "allocer_enum.h"
#include "config/config.h"
#include <assert.h>
#include <stdint.h>
typedef struct _Allocer_Base_VTable Allocer_Base_VTable;
typedef struct _Allocer_Block
{
    buffer_t random_id; // 块的idx,-1代表不可用
    void* ptr;          // 可用缓冲区的块的开头
} Allocer_Block;
// 要继承的基类
typedef struct _Allocer_Base
{
    Allocer_Base_VTable* m_vtable;
    // 分配内存的缓冲区
    void* m_buffer;
    Allocer_Buf_Source m_source;
} Allocer_Base;
typedef Allocer_Base* Allocer_Base_Ptr;
typedef Allocer_Block* Allocer_Block_Ptr;
/**
 * @brief 初始化分配器,设置分配器分配内存的缓冲区
 *
 * @param allocer_ptr
 * @param buf
 * @param buf_size
 * @return Allocer_Err
 *
 * @note 注意,buf_size 要比真正的缓冲区大 16 bytes，用于储存元数据。
 * @note SOURCE_SYSTEM会向系统申请buf_size的堆内存
 */
typedef Allocer_Err (*allocer_init)(Allocer_Base_Ptr, void*, buffer_t);
/**
 * @brief 处理析构前的逻辑，释放内存之类的
 *
 * @param allocer_ptr
 *
 * @note 注意,Allocer_Buf_Source为 SOURCE_SYSTEM 时，必须调用该函数
 */
typedef void (*allocer_deinit)(Allocer_Base_Ptr);
typedef Allocer_Err (*allocer_borrow)(Allocer_Base_Ptr, Allocer_Block_Ptr);
typedef void (*allocer_return_back)(Allocer_Base_Ptr, Allocer_Block_Ptr);
// 虚函数表
typedef struct _Allocer_Base_VTable
{
    allocer_init m_init;
    allocer_deinit m_deinit;
    allocer_borrow m_borrow;
    allocer_return_back m_return_back;
} Allocer_Base_VTable;
[[nodiscard]] static inline int Allocer_Is_Block_Invalid(Allocer_Block_Ptr block_ptr)
{
    assert(block_ptr != NULL);
    return block_ptr->random_id == ERR_BLOCK_ID;
}
#endif // !ROBOCOPY_TOOL_ALLOCER_ALLOCER_H
