#ifndef _LOG_H
#define _LOG_H

void event_err(int eval, const char* fmt, ...);
void event_warn(const char* fmt, ...);

void event_errx(int eval, const char* fmt, ...);
void event_warnx(const char* fmt, ...);
void event_msgx(const char* fmt, ...);
void event_debugx(const char* fmt, ...);

#endif
