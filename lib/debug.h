#ifndef _DEBUG_LIBRARY_H_
#define _DEBUG_LIBRARY_H_

enum debug_mode {
	DEBUG_OFF,
	DEBUG_ON,
};

/** デバッグ出力関数。ファイル名と行数出力のために関数マクロで実装している */
#define debug_print(...)                                             \
        do {                                                         \
                if (get_debug_mode() == DEBUG_ON) {                  \
                        debug_mutex_lock();                          \
                        set_filename_and_line(__FILE__, __LINE__);   \
                        debug_print_internal(__VA_ARGS__);           \
                        debug_mutex_unlock();                        \
                }                                                    \
        } while (0)

/** デバッグモードの変更。 */
extern void set_debug_mode(enum debug_mode mode);

/** 以下の関数はユーザが直接呼ぶことを想定していない。
 * debug_printを関数マクロで実装する必要があり、グローバルに公開しないといけない
 * 制限上、以下の関数も公開するしか手段がないため、ここで宣言する */
extern enum debug_mode get_debug_mode(void);
extern void debug_mutex_lock(void);
extern void debug_mutex_unlock(void);
extern void set_filename_and_line(const char *filename, int line);
extern void debug_print_internal(const char *format, ...);

#endif
