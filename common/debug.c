#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include <time.h>

#include "debug.h"

#define DEBUG_LOG_MESSAGE_LENGTH 512
#define DEBUG_LOG_HEADER_LENGTH 90

enum debug_mode {
	DEBUG_OFF,
	DEBUG_ON,
};

static enum debug_mode g_debug_mode = DEBUG_OFF;
static const char *g_filename;
static int g_line_number;
static pthread_mutex_t debug_mtx = PTHREAD_MUTEX_INITIALIZER;
static FILE *g_log_file_ptr;

int is_enable_debug(void)
{
	return g_debug_mode == DEBUG_ON;
}

void debug_mutex_lock(void)
{
	pthread_mutex_lock(&debug_mtx);
}

void debug_mutex_unlock(void)
{
	pthread_mutex_unlock(&debug_mtx);
}

void set_filename_and_line(const char *filename, int line)
{
	g_filename = filename;
	g_line_number = line;
}

void debug_print_internal(const char *format, ...)
{
	va_list ap;
	struct timespec ts;
	struct tm tm;
	char message[DEBUG_LOG_MESSAGE_LENGTH + 1] = { 0 };
	char header[DEBUG_LOG_HEADER_LENGTH + 1] = { 0 };

	/* 現在時刻取得 */
	clock_gettime(CLOCK_REALTIME, &ts);
	localtime_r(&ts.tv_sec, &tm);

	va_start(ap, format);
	vsnprintf(message, sizeof(message), format, ap);
	va_end(ap);

	snprintf(header, sizeof(header), "%d/%02d/%02d %02d:%02d:%02d.%06ld %lu %s:%d",
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ts.tv_nsec,
		(unsigned long)pthread_self(), g_filename, g_line_number);
	fprintf(g_log_file_ptr, "%*s %s\n", -DEBUG_LOG_HEADER_LENGTH, header, message);
	fflush(g_log_file_ptr);
}

/** デバッグ初期化処理. */
int debug_initialize(const char *log_filename)
{
	if ((g_log_file_ptr = fopen(log_filename, "a")) == NULL) {
		return -1;
	}
	g_debug_mode = DEBUG_ON;
	return 0;
}

/** デバッグ終了処理 */
void debug_finalize()
{
	g_debug_mode = DEBUG_OFF;
	fclose(g_log_file_ptr);
}
