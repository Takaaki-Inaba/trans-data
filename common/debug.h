#ifndef _DEBUG_LIBRARY_H_
#define _DEBUG_LIBRARY_H_

#include <string.h>
#include <errno.h>

/** デバッグ出力関数。ファイル名と行数出力のために関数マクロで実装している */
#define debug_print(...)                                             \
        do {                                                         \
                if (is_enable_debug()) {                             \
                        debug_mutex_lock();                          \
                        set_filename_and_line(__FILE__, __LINE__);   \
                        debug_print_internal(__VA_ARGS__);           \
                        debug_mutex_unlock();                        \
                }                                                    \
        } while (0)

#define debug_perror(func)                                           \
	do {                                                         \
		char tmp[256];                                       \
		strerror_r(errno, tmp, sizeof(tmp));                 \
		debug_print("%s error: %s", #func, tmp);             \
	} while (0)

/**
 * デバッグ初期化処理.
 * debug_printを呼ぶ前に本関数を呼ぶこと.
 * またデバッグを終了する時は、debug_finalizeを呼ぶこと
 *
 * @param[in] log_filename 出力するログファイル名
 *
 * @return
 * 	0  成功
 * 	-1 エラー
 **/
extern int debug_initialize(const char *log_filename);

/** デバッグ終了処理 */
extern void debug_finalize(void);


/** 以下の関数はユーザが直接呼ぶことを想定していない。
 * debug_printを関数マクロで実装する必要があり、グローバルに公開しないといけない
 * 制限上、以下の関数も公開するしか手段がないため、ここで宣言する */
extern int is_enable_debug(void);
extern void debug_mutex_lock(void);
extern void debug_mutex_unlock(void);
extern void set_filename_and_line(const char *filename, int line);
extern void debug_print_internal(const char *format, ...);

#endif
