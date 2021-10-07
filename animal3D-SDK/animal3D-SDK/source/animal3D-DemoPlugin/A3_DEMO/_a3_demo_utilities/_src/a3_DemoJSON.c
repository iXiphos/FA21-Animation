#include "../a3_DemoJSON.h"
#include "../a3_DemoUtils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct a3_JSONToken a3_JSONToken;
typedef enum a3_JSONTokenType a3_JSONTokenType;
typedef struct a3_JSONLexResult a3_JSONLexResult;
typedef struct a3_JSONLexState a3_JSONLexState;

enum a3_JSONTokenType {
    A3_JSONTOK_NONE,
    A3_JSONTOK_CHAR,
    A3_JSONTOK_INT,
    A3_JSONTOK_FLOAT,
    A3_JSONTOK_STRING,
};

struct a3_JSONToken {
    a3_JSONTokenType type;

    union {
        a3i64 token_int;
        double token_float;
        const char* token_string;
        char token_char;
    };
};

struct a3_JSONLexResult {
    a3ui32 count;
    a3ui32 capacity;
    a3_JSONToken* tokens;
};

struct a3_JSONLexState {
    a3ui32 count;
    a3ui32 capacity;
    a3_JSONToken* tokens;
    const char* ptr;
};


/*
Reads json in from file
*/
a3i32 a3readJSONFromFile(const char* path)
{
    char* buf;
    a3ReadFileIntoMemory(path, &buf);

    return a3readJSONFromString(buf);
}

void json_lex_next(a3_JSONLexState* state);
void json_lex_append(a3_JSONLexState* state, a3_JSONToken token);

a3i32 a3readJSONFromString(const char* buffer)
{
    a3_JSONLexState state[1];
    state->ptr = buffer;

    a3AllocArray(state->tokens, 10, a3_JSONToken);
    state->capacity = 10;
    state->count = 0;
    

    // while not eof
    while (*state->ptr != 0) {
        json_lex_next(state);
    }

    return -1;
}


/*
Lexatizer:
break the string input into the parts
*/

static char tmp_string[1024 * 1024];




void json_lex_string(a3_JSONLexState* state);
void json_lex_num(a3_JSONLexState* state);
void json_lex_char(a3_JSONLexState* state);

void json_lex_next(a3_JSONLexState* state) {

    //printf("|%c|\n", *ptr);
    switch (*state->ptr) {
    case 0: // EOF
        break;
    case '"':
        json_lex_string(state); break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '-':
    case '.':
        json_lex_num(state); break;
    case ',':
    case '[':
    case ']':
    case '{':
    case '}':
    case ':':
       json_lex_char(state); break;
    default:
        state->ptr++; break;
    }
}


void json_lex_print_tok(a3_JSONToken tok) {
    switch (tok.type) {
    case A3_JSONTOK_STRING:
        puts(tok.token_string);
        break;
    case A3_JSONTOK_INT:
        printf("%lli\n", tok.token_int);
        break;
    case A3_JSONTOK_CHAR:
        printf("%c\n", tok.token_char);
        break;
    case A3_JSONTOK_FLOAT:
        printf("%lf\n", tok.token_float);
        break;
    case A3_JSONTOK_NONE:
        puts("EMPTY_TOKEN");
        break;
    default:
        puts("UNKNOWN");
        break;
    }
}


void json_lex_append_str(a3_JSONLexState* state, char* str) {
    //todo: realocate strings into big buffer?
    json_lex_append(state, (a3_JSONToken) { .type = A3_JSONTOK_STRING, .token_string = str });
}

void json_lex_append_float(a3_JSONLexState* state, double f) {
    json_lex_append(state, (a3_JSONToken) { .type = A3_JSONTOK_FLOAT, .token_float = f });
}

void json_lex_append_int(a3_JSONLexState* state, a3i64 i) {
    json_lex_append(state, (a3_JSONToken) { .type = A3_JSONTOK_INT, .token_int = i });
}


void json_lex_append(a3_JSONLexState* state, a3_JSONToken tok) {
    if (state->count >= state->capacity) {
        // TOOD: better growth 
        state->capacity += 10;
        a3ResizeArray(state->tokens, state->capacity, a3_JSONToken);
    }
    json_lex_print_tok(tok);
    state->tokens[state->count] = tok;

    state->count++;
}

void json_lex_char(a3_JSONLexState* state) {
    char c = *(state->ptr++);
    json_lex_append(state, (a3_JSONToken) { .type = A3_JSONTOK_CHAR, .token_char = c });
}

void json_lex_string(a3_JSONLexState* state) {


    state->ptr++;

    a3ui32 size = 0;

    char* write = tmp_string;
    do {
        char c = *state->ptr;
        // string too big
        if (size > 1024 * 1024) { puts("ERROR: json string was too big"); exit(1); }
        if (c == '"') c = 0;
        // escape characters
        if (c == '\\') {
            state->ptr++;
            /*
                    %x22 /          ; "    quotation mark  U+0022
                    %x5C /          ; \    reverse solidus U+005C
                    %x2F /          ; /    solidus         U+002F
                    %x62 /          ; b    backspace       U+0008
                    %x66 /          ; f    form feed       U+000C
                    %x6E /          ; n    line feed       U+000A
                    %x72 /          ; r    carriage return U+000D
                    %x74 /          ; t    tab             U+0009
                    %x75 4HEXDIG )  ; uXXXX                U+XXXX
            */
            switch (*state->ptr) {
            case '"':
            case '\\':
            case '/':
                c = *state->ptr; break;
            case 'b':
                c = '\b'; break;
            case 'f':
                c = '\f'; break;
            case 'n':
                c = '\n'; break;
            case 'r':
                c = '\r'; break;
            case 't':
                c = '\t'; break;
            case 'u':
                // TODO: parse hex
                break;
            }
        }

        *write = c;
        // eof or quotation mark will be eof
        if (c == 0) break;
    } while (size++, write++, state->ptr++);


    char* str = malloc(size);
    memcpy(str, tmp_string, size);

    // step over the end quotation mark
    if (*state->ptr == '"') state->ptr++;

   json_lex_append_str(state, str);
}

void json_lex_num(a3_JSONLexState* state) {
    const char* start = state->ptr;

    signed char sign = 1;
    a3ui32 part_frac = 0;
    a3ui32 part_int = 0;
    a3ui32 part_exp = 0;
    a3ui32 frac_div = 0;

    // take care of negation first so we can treat any space as end of number later
    if (*state->ptr == '-') {
        sign = -1;
        do {
           state->ptr++;
        } while (*state->ptr == ' ');
    }

    do {
        char c = *state->ptr;

        if (c >= '0' && c <= '9') {
            a3ui32 i = '0' - c;
            if (frac_div == 0)
                part_int = (part_int * 10) + i;
            else {
                part_frac = (part_frac * 10) + i;
                frac_div *= 10;
            }
        }
        else if (c == '.') frac_div = 10;
        else break;
    } while (*state->ptr++ != 0);

    // no tractional digits == int
    if (frac_div == 0)
    {
        a3i64 i = (a3i64)sign * (a3i64)part_int;
        json_lex_append_int(state, i);
    } 
    else
    {
        double f = (double)sign * ((double)part_int + ((double)part_frac * (double)frac_div));
        json_lex_append_float(state, f);
    }
}
