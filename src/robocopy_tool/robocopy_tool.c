#include "allocer/allocer.h"
#include "allocer/allocer_base.h"
#include "config/config.h"
#include "stdio.h"
#include "tools/help_tool/help_tool.h"
#include <assert.h>
#include <shlwapi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ROBOCOPY_TOOL_VERSION "0.0.3"
#define BLOCK_PTR(block_name) (&(block_name))
#define ALLOC_BLOCK(def_allocer_p, block_name, step_err)                                                                   \
    do                                                                                                                     \
    {                                                                                                                      \
        (step_err) = (def_allocer_p)->m_base.m_vtable->m_borrow((Allocer_Base_Ptr)(def_allocer_p), BLOCK_PTR(block_name)); \
        Allocer_QuickPrintErr((step_err));                                                                                 \
        if ((step_err) != ALLOC_SUCCESS)                                                                                   \
        {                                                                                                                  \
            abort();                                                                                                       \
        }                                                                                                                  \
    } while (0)
static int ExitTool(Allocer_Ptr allocer_ptr,
                    Allocer_Block_Ptr block_source,
                    Allocer_Block_Ptr block_diraction,
                    Allocer_Block_Ptr block_cmd,
                    Allocer_Block_Ptr block_wc)
{
    if (allocer_ptr != NULL)
    {
        if (!Allocer_Is_Block_Invalid(block_source))
        {
            allocer_ptr->m_base.m_vtable->m_return_back((Allocer_Base_Ptr)allocer_ptr, block_source);
        }
        if (!Allocer_Is_Block_Invalid(block_diraction))
        {
            allocer_ptr->m_base.m_vtable->m_return_back((Allocer_Base_Ptr)allocer_ptr, block_diraction);
        }
        if (!Allocer_Is_Block_Invalid(block_cmd))
        {
            allocer_ptr->m_base.m_vtable->m_return_back((Allocer_Base_Ptr)allocer_ptr, block_cmd);
        }
        if (!Allocer_Is_Block_Invalid(block_wc))
        {
            allocer_ptr->m_base.m_vtable->m_return_back((Allocer_Base_Ptr)allocer_ptr, block_wc);
        }
        allocer_ptr->m_base.m_vtable->m_deinit((Allocer_Base_Ptr)allocer_ptr);

        (void)printf_s("Enter to exit\n");
        (void)getc(stdin);
        return 0;
    }
    return -1;
}
static_assert(DEFAULT_BUF_SIZE > (4ULL * DEFAULT_BLOCK_SIZE));
int main(void)
{
    Allocer_Ptr def_allocer_p = Allocer_Creat(SOURCE_DEFAULT);
    Allocer_Block block_source = {.random_id = ERR_BLOCK_ID, .ptr = NULL};
    Allocer_Block block_diraction = {.random_id = ERR_BLOCK_ID, .ptr = NULL};
    Allocer_Block block_cmd = {.random_id = ERR_BLOCK_ID, .ptr = NULL};
    Allocer_Block block_wc = {.random_id = ERR_BLOCK_ID, .ptr = NULL};
    Allocer_Err step_err = def_allocer_p->m_base.m_vtable->m_init((Allocer_Base_Ptr)def_allocer_p, NULL, DEFAULT_BUF_SIZE);
    (void)printf_s("[allocer init]:\n");
    Allocer_QuickPrintErr(step_err);
    if (step_err != ALLOC_SUCCESS)
    {
        abort();
    }
    (void)printf_s("[allocer block]:\n");
    ALLOC_BLOCK(def_allocer_p, block_source, step_err);
    ALLOC_BLOCK(def_allocer_p, block_diraction, step_err);
    ALLOC_BLOCK(def_allocer_p, block_cmd, step_err);
    ALLOC_BLOCK(def_allocer_p, block_wc, step_err);
    (void)printf_s(
        "[block init]: id: %zu,p: %p,size: %zu chars\n", block_source.random_id, block_source.ptr, DEFAULT_BLOCK_SIZE);
    (void)printf_s(
        "[block init]: id: %zu,p: %p,size: %zu chars\n", block_diraction.random_id, block_diraction.ptr, DEFAULT_BLOCK_SIZE);
    (void)printf_s("[block init]: id: %zu,p: %p,size: %zu chars\n", block_cmd.random_id, block_cmd.ptr, DEFAULT_BLOCK_SIZE);
    (void)printf_s("[block init]: id: %zu,p: %p,size: %zu chars\n", block_wc.random_id, block_wc.ptr, DEFAULT_BLOCK_SIZE);
    int cores = Num_CPU_Cores();
    int is_Calm = 0;
    do
    {
        Print_Cin_Clear(block_wc.ptr, "Is Calm to Print (Y/y n)>>");
    } while (!((((char*)block_wc.ptr)[0] == 'Y') || (((char*)block_wc.ptr)[0] == 'y') || (((char*)block_wc.ptr)[0] == 'n')));
    if (((char*)block_wc.ptr)[0] == 'n')
    {
        is_Calm = 1;
    }
    (void)printf_s("[Start]:\n");
    (void)printf_s("[Tool Verion]: %s\n", ROBOCOPY_TOOL_VERSION);
    (void)printf_s("[Calm Print]: %s\n", is_Calm ? "Yes" : "No");
    (void)printf_s("[CPU Cores]: %d ==> /MT: auto\n", cores);
    (void)printf_s(
        "=============================================================================================================="
        "==========\n");
    (void)printf_s(
        "=============================================================================================================="
        "==========\n");
    while (1)
    {
        (void)printf_s("Rules:\n\tpath can contain up to %zu characters.%s%s%s%s\n",
                       DEFAULT_BLOCK_SIZE / 2ULL - 32ULL,
                       "\n\t[1] Input \"exit\" or \"Exit\" to exit !",
                       "\n\t[2] Please wait to this exe print \"Complete!\".Then close this exe.",
                       "\n\t[3] Warning: One Chinese Character maybe 2 or 3 chars.",
                       "\n\t[4] Warning: Target diraction path MUST NOT be driver letter! such as(D:\\).\n",
                       "Note:\n\tThis tool Will copy all the subdiraction (including the empty diraction) ,\n\tand ignore the "
                       "NOEXITS target diraction!");
        Path_Data data = {0};
        do
        {
            Print_Cin_Clear(block_source.ptr, "Source path>>");
            data = Check_Path(block_source.ptr, block_wc.ptr, PATH_SOURCE, DEFAULT_BLOCK_SIZE / 2ULL);
            if (strstr(block_source.ptr, "exit") || strstr(block_source.ptr, "Exit"))
            {
                return ExitTool(def_allocer_p, &block_source, &block_diraction, &block_cmd, &block_wc);
            }
        } while (!Check_Data(data));
        do
        {
            Print_Cin_Clear(block_diraction.ptr, "Diraction path>>");
            data = Check_Path(block_diraction.ptr, block_wc.ptr, PATH_DIRACTION, DEFAULT_BLOCK_SIZE / 2ULL);
            if (strstr(block_diraction.ptr, "exit") || strstr(block_diraction.ptr, "Exit"))
            {
                return ExitTool(def_allocer_p, &block_source, &block_diraction, &block_cmd, &block_wc);
            }
        } while (!Check_Data(data));
        char* cmd = (char*)block_cmd.ptr;
        cmd[0] = '\0';
        (void)strcat(cmd, "ROBOCOPY \"");
        (void)strcat(cmd, block_source.ptr);
        (void)strcat(cmd, "\" \"");
        (void)strcat(cmd, block_diraction.ptr);
        (void)strcat(cmd, "\" /R:3 /W:1 /E /MT");
        if (cores > 0)
        {
            (void)strcat(cmd, ":");
            Int_Tran_Str(block_wc.ptr, Num_MT(cores, block_source.ptr, block_diraction.ptr));
            (void)strcat(cmd, block_wc.ptr);
        }
        if (is_Calm)
        {
            (void)strcat(cmd, " /NFL /NDL /NJH /NJS /NP");
        }
        (void)printf_s("[RUN cmd]: %s\n", cmd);
        FILE* r_file = _popen(cmd, "r");
        if (r_file == NULL)
        {
            (void)printf_s("[run ERROR]: Cannot RUN cmd %s\n", cmd);
            return ExitTool(def_allocer_p, &block_source, &block_diraction, &block_cmd, &block_wc);
        }
        while (fgets(block_wc.ptr, DEFAULT_BLOCK_SIZE, r_file))
        {
            (void)printf_s("%s", block_wc.ptr);
        }
        _pclose(r_file);
        (void)printf_s("Complete!\n\n");
    }
    (void)getc(stdin);
    return 0;
}