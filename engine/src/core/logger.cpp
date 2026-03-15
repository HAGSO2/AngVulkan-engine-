#include "logger.h"
#include "asserts.h"
#include "core/Astring.h"
#include "platform/platform.h"
#include "platform/filesystem.h"


#include <stdarg.h>

struct logger_system_state {
    file_handle log_file_handle;
};

static logger_system_state* state_ptr = nullptr;

void append_to_log_file(const char* message){
    if(state_ptr && state_ptr->log_file_handle.is_valid){
        // Since the message already contains a '\n', just write the bytes directly.
        u64 length = string_length(message);
        u64 written = 0;
        if(!filesystem_write(&state_ptr->log_file_handle, length, message, &written)){
            platform_console_write_error("ERROR writting to console.log", LOG_LEVEL_ERROR);
        }
    }
}

b8 initialize_logging() {
    state_ptr = new logger_system_state{};
    //Create new/wipe existing log file, then open it.
    if(!filesystem_open("console.log",FILE_MODE_WRITE, FALSE, &state_ptr->log_file_handle)){
        platform_console_write_error("ERROR: Unable to open console.log for writting.", LOG_LEVEL_ERROR);
        return FALSE;
    }

    KFATAL("A test message: %f", 3.14f);
    KERROR("A test message: %f", 3.14f);
    KWARN("A test message: %f", 3.14f);
    KINFO("A test message: %f", 3.14f);
    KDEBUG("A test message: %f", 3.14f);
    KTRACE("A test message: %f", 3.14f);

    return TRUE;
}

void shutdown_logging() {
    delete state_ptr;
    state_ptr = nullptr;
}

void log_output(log_level level, const char* message, ...) {
    //TODO: These string operations are all pretty slow. This needs to be 
    //moved to another thread
    const char* level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]:  ", "[INFO]:  ", "[DEBUG]: ", "[TRACE]: "};
    b8 is_error = level < LOG_LEVEL_WARN;

    // Technically imposes a 32k character limit on a single log entry, but...
    // DON'T DO THAT!
    const i32 msg_length = 32000;
    char out_message[msg_length];

    // Format original message.
    // NOTE: Oddly enough, MS's headers override the GCC/Clang va_list type with a "typedef char* va_list" in some
    // cases, and as a result throws a strange error here. The workaround for now is to just use __builtin_va_list,
    // which is the type GCC/Clang's va_start expects.
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    string_format(out_message, message, arg_ptr);
    va_end(arg_ptr);

    string_format(out_message, "%s%s\n", level_strings[level], out_message);

    // Platform-specific output.
    if (is_error) {
        platform_console_write_error(out_message, level);
    } else {
        platform_console_write(out_message, level);
    }

    append_to_log_file(out_message);
}

void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line) {
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}