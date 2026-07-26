//
// MIT License
//
// Copyright (c) 2021 Zoltán Máté
//

#ifndef TEST_H_MAIN
#define TEST_H_MAIN

#ifndef __BASE_FILE__
#error "Macro __BASE_FILE__ is not defined."
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int __test_h_run_test_case_in_separate_process(const char *path, const char *test_case)
{
    const int path_length = strlen(path);
    char command_line[path_length + 1 + strlen(test_case) + 1];

    strcpy(command_line, path);
    command_line[path_length] = ' ';
    strcpy(command_line + path_length + 1, test_case);

    const int result = system(command_line);

    printf(__BASE_FILE__ ":%s: %s\n", test_case, result ? "FAIL" : "PASS");

    return result;
}

#define __test_h_return_type void

#undef NDEBUG
#include <assert.h>

#define ASSERT(condition) assert(condition)

int main(int argc, const char *argv[]) {

    int exit_status = 0;

#include __BASE_FILE__

    return exit_status;
}

#undef TEST
#define TEST(test_case) __test_h_return_type test_case()

#elif ! defined (TEST_H_INSIDE_MAIN)
#define TEST_H_INSIDE_MAIN

#define TEST(test_case) \
    if (argc < 2) { \
        printf("%s\n", #test_case); \
    } \
    else if (!strcmp(argv[1], "--all")) { \
        exit_status = __test_h_run_test_case_in_separate_process(argv[0], #test_case) || exit_status; \
    } \
    else if (!strcmp(argv[1], #test_case)) { \
        __test_h_return_type test_case(); \
        test_case(); \
        return 0; \
    } \
    while (0)  // disable the following function body

#endif
