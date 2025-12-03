#include <stdio.h>
#include <stdlib.h>

#include "codegen/codegen.h"
#include "codegen/gen_temps.h"
#include "utils/error.h"
#include "utils/log.h"

extern int yyparse(void);
extern FILE* yyin;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Use: %s <file.cl> <logs.log>\n", argv[0]);
        return EXIT_FAILURE;
    }

    error_init();
    log_init(argv[2]);
    code_init(stdout);

    FILE* input_file = fopen(argv[1], "r");
    if (!input_file) {
        SourceLocation loc = {0, 0, argv[1]};
        REPORT_LEXICAL_ERROR(loc, "Can't open file '%s'", argv[1]);
        log_close();
        code_close();
        return EXIT_FAILURE;
    }

    yyin = input_file;
    printf("=== COMPILING ===\n");

    int parse_result = yyparse();
    if (parse_result == 0 && !has_errors()) {
        emit_halt();
        printf("=== COMPILATION COMPLETE ===\n");
    } else {
        printf("=== COMPILATION FAILED ===\n");
    }
    printf("   - Number of errores encoutered: %d\n", get_error_count());

    fclose(input_file);
    log_close();
    free_temps();
    code_close();

    return has_errors() ? EXIT_FAILURE : EXIT_SUCCESS;
}