#ifndef ROBOCOPY_TOOL_TOOLS_HELP_TOOL_HELP_TOOL_H
#define ROBOCOPY_TOOL_TOOLS_HELP_TOOL_HELP_TOOL_H
/**
 * @brief 判断一个字符是否为空字符
 *
 * @param c
 * @return unsigned char
 *
 * @note [space] \t \n \r \v \f
 */
unsigned char Isspace(const int c);
/**
 * @brief 线性遍历，判断一个字符串是否为空
 *
 * @param str
 * @return unsigned char
 *
 * @note O(n)
 */
unsigned char IsEmpty(const char* str);
/**
 * @brief 清空输入缓冲区
 *
 *
 * @note
 */
void ClearStdin(void);
/**
 * @brief 根据strlen获取size,并去除两侧""和空字符
 *
 * @param str
 *
 * @note
 */
void Clear_dqm_spaces(char* str);
/**
 * @brief 打印字符串，然后向buf从stdin读入,再清空stdin缓冲区
 *
 * @param buf
 * @param to_print
 *
 * @note fgets的 max_size 为 DEFAULT_BLOCK_SIZE - 1ULL
 */
void Print_Cin_Clear(char* buf, const char* to_print);
typedef struct _Path_Data
{
    // is source: 0x01
    // not file: 0x02
    // is exist 0x04
    int m_flag;
} Path_Data;
#define PATH_SOURCE    0x01
#define PATH_DIRACTION 0x00
/**
 * @brief 检查目录路径是否合法
 *
 * @param path
 * @param wc_path
 * @param is_source
 * @param wc_size
 * @return Path_Data
 *
 * @note
 */
[[nodiscard]] Path_Data Check_Path(const char* path, unsigned short* wc_path, int is_source, unsigned long long wc_size);
/**
 * @brief 检查数据是否符合标准
 *
 * @param data
 * @return int
 *
 * @note
 */
[[nodiscard]] int Check_Data(Path_Data data);
#endif // !ROBOCOPY_TOOL_TOOLS_HELP_TOOL_H