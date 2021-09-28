#include "../a3_DemoJSON.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct a3_JSONToken a3_JSONToken;
typedef enum a3_JSONTokenType a3_JSONTokenType;
typedef struct a3_JSONLexResult a3_JSONLexResult;

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
}

a3i32 a3readJSONFromFile(const char* path)
{
    return -1;
}



a3i32 a3readJSONFromString(const char* buffer)
{
    a3_JSONLexState state;

    while (*state.ptr != 0) {

    }

    return -1;
}


/*
Lexatizer:
break the string input into the parts
*/

static char tmp_string[1024 * 1024];

#define JSON_LEX_PARAMS \
  const char* ptr, a3_JSONToken* tokens, a3ui32 count, a3ui32 capacity


#define JSON_LEX_ARGS \
  ptr, tokens, count, capacity

a3_JSONLexResult json_lex_next(JSON_LEX_PARAMS);
a3_JSONLexResult json_lex_string(JSON_LEX_PARAMS);
a3_JSONLexResult json_lex_num(JSON_LEX_PARAMS);
a3_JSONLexResult json_lex_char(JSON_LEX_PARAMS);
a3_JSONLexResult json_lex_append(JSON_LEX_PARAMS, a3_JSONToken tok);

a3_JSONLexResult json_lex_next(JSON_LEX_PARAMS) {

    //printf("|%c|\n", *ptr);
    switch (*ptr) {
    case 0: // EOF
        return (a3_JSONLexResult) { .count = count, .capacity = capacity, .tokens = tokens };
    case '"':
        return json_lex_string(JSON_LEX_ARGS);
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
        return json_lex_num(JSON_LEX_ARGS);
    case ',':
    case '[':
    case ']':
    case '{':
    case '}':
    case ':':
        return json_lex_char(JSON_LEX_ARGS);
    default:
        ptr++;
        return json_lex_next(JSON_LEX_ARGS);
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

a3_JSONLexResult json_lex_append(JSON_LEX_PARAMS, a3_JSONToken tok) {
    if (count >= capacity) {
        // TOOD: better growth 
        capacity += 10;
        tokens = (a3_JSONToken*)realloc(tokens, sizeof(a3_JSONToken) * capacity);
        printf("realloc\n");
    }
    json_lex_print_tok(tok);
    tokens[count] = tok;

    count++;
    return json_lex_next(JSON_LEX_ARGS);

}

a3_JSONLexResult json_lex_append_str(JSON_LEX_PARAMS, char* str) {
    return json_lex_append(JSON_LEX_ARGS, (a3_JSONToken) { .type = A3_JSONTOK_STRING, .token_string = str });
}

a3_JSONLexResult json_lex_append_float(JSON_LEX_PARAMS, double f) {
    return json_lex_append(JSON_LEX_ARGS, (a3_JSONToken) { .type = A3_JSONTOK_FLOAT, .token_float = f });
}

a3_JSONLexResult json_lex_append_int(JSON_LEX_PARAMS, a3i64 i) {
    return json_lex_append(JSON_LEX_ARGS, (a3_JSONToken) { .type = A3_JSONTOK_INT, .token_int = i });
}
a3_JSONLexResult json_lex_append_char(JSON_LEX_PARAMS, char c) {
    return json_lex_append(JSON_LEX_ARGS, (a3_JSONToken) { .type = A3_JSONTOK_CHAR, .token_char = c });
}

a3_JSONLexResult json_lex_string(JSON_LEX_PARAMS) {


    ptr++;

    a3ui32 size = 0;

    char* write = tmp_string;
    do {
        char c = *ptr;
        // string too big
        if (size > 1024 * 1024) { puts("ERROR: json string was too big"); exit(1); }
        if (c == '"') c = 0;
        // escape characters
        if (c == '\\') {
            ptr++;
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
            switch (*ptr) {
            case '"':
            case '\\':
            case '/':
                c = *ptr; break;
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
    } while (size++, write++, ptr++);


    char* str = malloc(size);
    memcpy(str, tmp_string, size);

    // step over the end quotation mark
    if (*ptr == '"') ptr++;

    return json_lex_append_str(JSON_LEX_ARGS, str);
}


a3_JSONLexResult json_lex_num(JSON_LEX_PARAMS) {
    const char* start = ptr;

    signed char sign = 1;
    a3ui32 part_frac;
    a3ui32 part_int;
    a3ui32 part_exp;
    a3ui32 frac_div;

    // take care of negation first so we can treat any space as end of number later
    if (*ptr == '-') {
        sign = -1;
        do {
            ptr++;
        } while (*ptr == ' ');
    }

    do {
        char c = *ptr;

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
    } while (*ptr++ != 0);

    // no tractional digits == int
    if (frac_div == 0)
        return json_lex_append_int(JSON_LEX_ARGS, (a3i64)sign * (a3i64)part_int);



    double f = (double)sign * ((double)part_int + ((double)part_frac * (double)frac_div));
    return json_lex_append_float(JSON_LEX_ARGS, f);
}


a3_JSONLexResult json_lex_char(JSON_LEX_PARAMS) {
    char c = *(ptr++);
    return json_lex_append(JSON_LEX_ARGS, (a3_JSONToken) { .type = A3_JSONTOK_CHAR, .token_char = c });
}



int main() {
    const char* DATA = "{"
        "\"glossary\": {"
        "    \"title\": \"example glossary\","
        "\"GlossDiv\": {"
        "        \"title\": \"S\","
        "	\"GlossList\": {"
        "            \"GlossEntry\": {"
        "                \"ID\": \"SGML\","
        "			\"SortAs\": \"SGML\","
        "			\"GlossTerm\": \"Standard Generalized Markup Language\","
        "			\"Acronym\": \"SGML\","
        "			\"Abbrev\": \"ISO 8879:1986\","
        "			\"GlossDef\": {"
        "                    \"para\": \"A meta-markup language, used to create markup languages such as \\\"DocBook.\","
        "				\"GlossSeeAlso\": [\"GML\", \"XML\"]"
        "                },"
        "			\"GlossSee\": \"markup\""
        "            }"
        "        }"
        " \"number\":13.4,"
        " \"int\": 16, "
        "    }"
        "}"
        "}";
    puts(DATA);
    json_lex_next(DATA, NULL, 0, 0);




    return 0;
}