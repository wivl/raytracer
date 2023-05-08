#ifndef _LOG_HPP_
#define _LOG_HPP_
#include <iostream>
#include <iomanip>

void print_log(const char *type, const char *from, const char *content);
void print_progress(int completed, int total, int width);

#endif
