#ifndef ROBOCOPY_TOOL_ALLOCER_ALLOCER_H
#define ROBOCOPY_TOOL_ALLOCER_ALLOCER_H
#include "allocer_base.h"
#include "config/config.h"
typedef struct _Allocer
{
    Allocer_Base m_base;
    void* m_head; // 空闲链表的表头
} Allocer;
typedef Allocer* Allocer_Ptr;
void Allocer_QuickPrintErr(Allocer_Err err);
/**
 * @brief 该函数主要用于创建一个单例,服务于SOURCE_DEFAULT
 *
 * @param source_type
 * @return Allocer_Ptr
 *
 * @note 注意，因为SOURCE_DEFAULT共用一个缓冲区,其它SOURCE_TYPE返回NULL
 */
Allocer_Ptr Allocer_Creat(Allocer_Buf_Source source_type);
/**
 * @brief 该函数用于绑定虚函数表
 *
 * @param allocer_ptr
 *
 * @note
 */
void Allocer_Bind_VTable(Allocer_Base_Ptr allocer_ptr);
/**
 * @brief 检查该分配器的缓冲区是否以满
 *
 * @param allocer
 * @return true
 * @return false
 *
 * @note
 */
[[nodiscard]] int Allocer_Full(Allocer_Ptr allocer_ptr);
/**
 * @brief 获取可用缓冲区大小
 *
 * @param allocer_ptr
 * @return buffer_t
 *
 * @note
 */
[[nodiscard]] buffer_t Allocer_Capacity(Allocer_Ptr allocer_ptr);
/**
 * @brief 获取总共的缓冲块的数量
 *
 * @param allocer_ptr
 * @return buffer_t
 *
 * @note
 */
[[nodiscard]] buffer_t Allocer_Block_Total(Allocer_Ptr allocer_ptr);
#endif // !ROBOCOPY_TOOL_ALLOCER_ALLOCER_H
