#include "allocer.h"
#include "allocer_base.h"
#include "config/config.h"
#include "tools/rand_tool/rand_tool.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static char default_buffer[DEFAULT_BUF_SIZE] = {0};
static Allocer_Base_VTable allocer_g_vtable;
static Allocer_Err Allocer_init(Allocer_Base_Ptr allocer_ptr, void* buf, buffer_t buf_size)
{
    assert(allocer_ptr != NULL);
    if ((buf_size <= 16ULL) || (DEFAULT_BLOCK_SIZE > (buf_size - 16ULL)))
    {
        return BUF_SIZE_SMALLER_THAN_BLOCK_SIZE;
    }
    Allocer_Ptr allocer = (Allocer_Ptr)allocer_ptr;
    void* buffer = NULL;
    buffer_t using_size = 0ULL;
    switch (allocer->m_base.m_source)
    {
    case SOURCE_DEFAULT:
    {
        using_size = DEFAULT_BUF_SIZE - 16ULL;
        buffer = default_buffer;
        break;
    }
    case SOURCE_USER:
    {
        if (buf == NULL)
        {
            return NO_BUFFER;
        }
        using_size = buf_size - 16ULL;
        buffer = buf;
        break;
    }
    case SOURCE_SYSTEM:
    {
        char* sys_buf = (char*)malloc(buf_size);
        using_size = buf_size - 16ULL;
        if (sys_buf == NULL)
        {
            return SYSTEM_ALLOC_ERR;
        }
        buffer = sys_buf;
        break;
    }
    default:
        return UNKNOWN_BUF_SOURCE;
    }
    memcpy(buffer, &using_size, BUFFER_T_SIZE);
    buffer_t block_total = using_size / DEFAULT_BLOCK_SIZE;
    memcpy((char*)buffer + BUFFER_T_SIZE, &block_total, BUFFER_T_SIZE);
    allocer->m_head = allocer->m_base.m_buffer = (char*)buffer + 16ULL;
    void* next_block = NULL;
    for (buffer_t i = 1ULL; i < block_total; i++) // Write free link table
    {
        next_block = (char*)allocer->m_base.m_buffer + i * DEFAULT_BLOCK_SIZE;
        memcpy(allocer->m_head, &next_block, PTR_T_SIZE);
        allocer->m_head = next_block;
    }
    next_block = NULL;
    memcpy(allocer->m_head, &next_block, PTR_T_SIZE);
    allocer->m_head = allocer->m_base.m_buffer;
    return ALLOC_SUCCESS;
}
static void Allocer_Deinit(Allocer_Base_Ptr allocer_ptr)
{
    assert(allocer_ptr != NULL);
    Allocer_Ptr allocer = (Allocer_Ptr)allocer_ptr;

    switch (allocer->m_base.m_source)
    {
    case SOURCE_SYSTEM:
    {
        free((char*)allocer->m_base.m_buffer - 16ULL);
        [[fallthrough]];
    }
    case SOURCE_DEFAULT:
    case SOURCE_USER:
    {
        allocer->m_base.m_buffer = NULL;
        allocer->m_head = NULL;
        allocer->m_base.m_vtable = NULL;
        allocer->m_base.m_source = SOURCE_NONE;
        break;
    }
    default:
        break;
    }
}
static Allocer_Err Allocer_Borrow(Allocer_Base_Ptr allocer_ptr, Allocer_Block_Ptr block_ptr)
{
    assert(allocer_ptr != NULL);
    assert(block_ptr != NULL);
    Allocer_Ptr allocer = (Allocer_Ptr)allocer_ptr;
    switch (allocer->m_base.m_source)
    {
    case SOURCE_DEFAULT:
    case SOURCE_USER:
    case SOURCE_SYSTEM:
    {
        if (Allocer_Full(allocer))
        {
            return NO_FREE_BLOCK;
        }
        block_ptr->random_id = Tool_GetRandU64();
        block_ptr->ptr = allocer->m_head;
        void* next_ptr = NULL;
        memcpy(&next_ptr, allocer->m_head, PTR_T_SIZE);
        allocer->m_head = next_ptr;
        break;
    }
    default:
        return UNKNOWN_BUF_SOURCE;
    }
    return ALLOC_SUCCESS;
}
static void Allocer_Return_Back(Allocer_Base_Ptr allocer_ptr, Allocer_Block_Ptr block_ptr)
{
    assert(allocer_ptr != NULL);
    assert(block_ptr != NULL);
    Allocer_Ptr allocer = (Allocer_Ptr)allocer_ptr;
    if (Allocer_Is_Block_Invalid(block_ptr))
    {
        return;
    }
    switch (allocer->m_base.m_source)
    {
    case SOURCE_DEFAULT:
    case SOURCE_USER:
    case SOURCE_SYSTEM:
    {
        block_ptr->random_id = ERR_BLOCK_ID;
        void* last_ptr = block_ptr->ptr;
        block_ptr->ptr = NULL;
        void* next = allocer->m_head;
        memcpy(last_ptr, &next, PTR_T_SIZE);
        allocer->m_head = last_ptr;
        break;
    }
    default:
        break;
    }
}
Allocer_Ptr Allocer_Creat(Allocer_Buf_Source source_type)
{
    switch (source_type)
    {
    case SOURCE_DEFAULT:
    {
        static Allocer signal_allocer = {0};
        signal_allocer.m_base.m_source = source_type;
        signal_allocer.m_base.m_vtable = &allocer_g_vtable;
        return &signal_allocer;
    }
    case SOURCE_USER:
    case SOURCE_SYSTEM:
        break;
    default:
        break;
    }
    return NULL;
}
void Allocer_Bind_VTable(Allocer_Base_Ptr allocer_ptr)
{
    assert(allocer_ptr != NULL);
    allocer_ptr->m_vtable = &allocer_g_vtable;
}
static Allocer_Base_VTable allocer_g_vtable
    = {.m_borrow = Allocer_Borrow, .m_return_back = Allocer_Return_Back, .m_init = Allocer_init, .m_deinit = Allocer_Deinit};
int Allocer_Full(Allocer_Ptr allocer_ptr)
{
    assert(allocer_ptr != NULL);
    return allocer_ptr->m_head == NULL;
}
void Allocer_QuickPrintErr(Allocer_Err err)
{
    switch (err)
    {
    case ALLOC_SUCCESS:
    {
        (void)printf_s("%s\n", "[Allocer Err]:ALLOC Success!");
        break;
    }
    case BUF_SIZE_SMALLER_THAN_BLOCK_SIZE:
    {
        (void)printf_s("%s\n", "[Allocer Err]: Buffer size is SMALLER THAN Block size!");
        break;
    }
    case NO_BUFFER:
    {
        (void)printf_s("%s\n", "[Allocer Err]: Buffer is NULL!");
        break;
    }
    case UNKNOWN_BUF_SOURCE:
    {
        (void)printf_s("%s\n", "[Allocer Err]: Unknown Buffer Source!");
        break;
    }
    case NO_FREE_BLOCK:
    {
        (void)printf_s("%s\n", "[Allocer Err]: Buffer is full!");
        break;
    }
    case SYSTEM_ALLOC_ERR:
    {
        (void)printf_s("%s\n", "[Allocer Err]: System alloc Err!");
        break;
    }
    default:
    {
        (void)printf_s("%s\n", "[Allocer Err]: Unknown Err!");
    }
    }
}
buffer_t Allocer_Block_Total(Allocer_Ptr allocer_ptr)
{
    assert(allocer_ptr != NULL);
    buffer_t block_total = 0ULL;
    memcpy(&block_total, (char*)allocer_ptr->m_base.m_buffer - 8ULL, BUFFER_T_SIZE);
    return block_total;
}
buffer_t Allocer_Capacity(Allocer_Ptr allocer_ptr)
{
    assert(allocer_ptr != NULL);
    buffer_t capacity = 0ULL;
    memcpy(&capacity, (char*)allocer_ptr->m_base.m_buffer - 16ULL, BUFFER_T_SIZE);
    return capacity;
}