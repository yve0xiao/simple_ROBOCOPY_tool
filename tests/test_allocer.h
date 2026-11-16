#ifndef ROBOCOPY_TOOL_TESTS_TEST_ALLOCER_H
#define ROBOCOPY_TOOL_TESTS_TEST_ALLOCER_H
#ifdef TEST_ALLOCER
#undef TEST_ALLOCER
static int run_allocer_borrow_return_cycle(Allocer* allocer, const char* tag)
{
    Allocer_Block block1 = {0};
    Allocer_Block block2 = {0};
    Allocer_Block block3 = {0};

    if (allocer->m_base.m_vtable->m_borrow((Allocer_Base*)allocer, &block1) == ALLOC_SUCCESS)
    {
        printf_s("[Test Allocer %s]: alloc block1 SUCCESS!\n", tag);
        printf_s("block id: %llu , block ptr: %p\n", (unsigned long long)block1.random_id, block1.ptr);
    }
    if (allocer->m_base.m_vtable->m_borrow((Allocer_Base*)allocer, &block2) == ALLOC_SUCCESS)
    {
        printf_s("[Test Allocer %s]: alloc block2 SUCCESS!\n", tag);
        printf_s("block id: %llu , block ptr: %p\n", (unsigned long long)block2.random_id, block2.ptr);
    }
    if (allocer->m_base.m_vtable->m_borrow((Allocer_Base*)allocer, &block3) == ALLOC_SUCCESS)
    {
        printf_s("[Test Allocer %s]: alloc block3 SUCCESS!\n", tag);
        printf_s("block id: %llu , block ptr: %p\n", (unsigned long long)block3.random_id, block3.ptr);
    }

    int is_FULL = Allocer_Full(allocer);
    if (is_FULL)
    {
        printf_s("[Test Allocer %s]: allocer FULL!\n", tag);

        allocer->m_base.m_vtable->m_return_back((Allocer_Base*)allocer, &block1);
        allocer->m_base.m_vtable->m_return_back((Allocer_Base*)allocer, &block2);

        Allocer_Block block4 = {0};
        if (allocer->m_base.m_vtable->m_borrow((Allocer_Base*)allocer, &block4) == ALLOC_SUCCESS)
        {
            printf_s("[Test Allocer %s]: alloc block4 SUCCESS!\n", tag);
            printf_s("block id: %llu , block ptr: %p\n", (unsigned long long)block4.random_id, block4.ptr);
        }

        allocer->m_base.m_vtable->m_return_back((Allocer_Base*)allocer, &block3);
        allocer->m_base.m_vtable->m_return_back((Allocer_Base*)allocer, &block4);

        Allocer_Block block5 = {0};
        if (allocer->m_base.m_vtable->m_borrow((Allocer_Base*)allocer, &block5) == ALLOC_SUCCESS)
        {
            printf_s("[Test Allocer %s]: alloc block5 SUCCESS!\n", tag);
            printf_s("block id: %llu , block ptr: %p\n", (unsigned long long)block5.random_id, block5.ptr);
        }
        Allocer_Block block6 = {0};
        if (allocer->m_base.m_vtable->m_borrow((Allocer_Base*)allocer, &block6) == ALLOC_SUCCESS)
        {
            printf_s("[Test Allocer %s]: alloc block6 SUCCESS!\n", tag);
            printf_s("block id: %llu , block ptr: %p\n", (unsigned long long)block6.random_id, block6.ptr);
        }
        Allocer_Block block7 = {0};
        if (allocer->m_base.m_vtable->m_borrow((Allocer_Base*)allocer, &block7) == ALLOC_SUCCESS)
        {
            printf_s("[Test Allocer %s]: alloc block7 SUCCESS!\n", tag);
            printf_s("block id: %llu , block ptr: %p\n", (unsigned long long)block7.random_id, block7.ptr);
        }

        if (Allocer_Full(allocer))
        {
            printf_s("[Test Allocer %s]: allocer FULL TOO!\n", tag);
            allocer->m_base.m_vtable->m_return_back((Allocer_Base*)allocer, &block6);
            allocer->m_base.m_vtable->m_return_back((Allocer_Base*)allocer, &block7);
            allocer->m_base.m_vtable->m_return_back((Allocer_Base*)allocer, &block5);
        }
    }
    else
    {
        printf_s("[Test Allocer %s]: NOT FULL after 3 borrows, return path skipped\n", tag);
    }
    return 0;
}
static int test_default_allocer(void)
{
    Allocer allocer = {0};
    allocer.m_base.m_source = SOURCE_DEFAULT;
    Allocer_Bind_VTable(&allocer.m_base);

    Allocer_Err err = allocer.m_base.m_vtable->m_init((Allocer_Base*)&allocer, NULL, (buffer_t)DEFAULT_BUF_SIZE);
    if (err != ALLOC_SUCCESS)
    {
        Allocer_QuickPrintErr(err);
        return -1;
    }

    printf_s("[Test Allocer DEFAULT]: init SUCCESS!\n");
    int ret = run_allocer_borrow_return_cycle(&allocer, "DEFAULT");

    allocer.m_base.m_vtable->m_deinit((Allocer_Base*)&allocer);
    return ret;
}

static int test_user_allocer(void)
{
    static unsigned char user_buffer[DEFAULT_BUF_SIZE] = {0};

    Allocer allocer = {0};
    allocer.m_base.m_source = SOURCE_USER;
    Allocer_Bind_VTable(&allocer.m_base);

    Allocer_Err err = allocer.m_base.m_vtable->m_init((Allocer_Base*)&allocer, user_buffer, (buffer_t)sizeof(user_buffer));
    if (err != ALLOC_SUCCESS)
    {
        Allocer_QuickPrintErr(err);
        return -1;
    }

    printf_s("[Test Allocer USER]: init SUCCESS!\n");
    int ret = run_allocer_borrow_return_cycle(&allocer, "USER");

    allocer.m_base.m_vtable->m_deinit((Allocer_Base*)&allocer);
    return ret;
}
static int test_system_allocer(void)
{
    Allocer allocer = {0};
    allocer.m_base.m_source = SOURCE_SYSTEM;
    Allocer_Bind_VTable(&allocer.m_base);

    Allocer_Err err = allocer.m_base.m_vtable->m_init((Allocer_Base*)&allocer, NULL, DEFAULT_BUF_SIZE);
    if (err != ALLOC_SUCCESS)
    {
        Allocer_QuickPrintErr(err);
        return -1;
    }

    printf_s("[Test Allocer SYSTEM]: init SUCCESS!\n");
    int ret = run_allocer_borrow_return_cycle(&allocer, "SYSTEM");

    allocer.m_base.m_vtable->m_deinit((Allocer_Base*)&allocer);
    return ret;
}
#endif // DEBUG
#endif // !ROBOCOPY_TOOL_TESTS_TEST_ALLOCER_H