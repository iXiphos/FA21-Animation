#include "../a3_DemoJSON.h"
#include "../a3_DemoUtils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include  <math.h>




a3boolean a3JSONFindObjValue(a3_JSONValue obj, const char* key, a3_JSONValue* val_out) {
    if (obj.type != JSONTYPE_OBJ) {
        return false;
    }
    for (a3ui32 i = 0; i < obj.obj.length; i++) {
        if (strcmp(key, obj.obj.keys[i]) == 0) {
            *val_out = obj.obj.values[i];
            return true;
        }
    }

    return false;
}

a3boolean a3JSONGetNum(a3_JSONValue value, double* num_out) {
    if (value.type == JSONTYPE_NUM) {
        *num_out = value.num;
        return true;
    }
    return false;
}

a3boolean a3JSONGetStr(a3_JSONValue value, const char** str_out) {
    if (value.type == JSONTYPE_STR) {
        *str_out = value.str;
        return true;
    }
    return false;
}

a3boolean a3JSONGetBoolean(a3_JSONValue value, a3boolean* bool_out) {
    if (value.type == JSONTYPE_TRUE) {
        *bool_out = 1;
        return true;
    }
    if (value.type == JSONTYPE_FALSE) {
        *bool_out = 0;
        return true;
    }
    return false;
}
















typedef struct a3_JSONToken a3_JSONToken;
typedef enum a3_JSONTokenType a3_JSONTokenType;
typedef struct a3_JSONParseState a3_JSONParseState;
typedef struct a3_JSONLexState a3_JSONLexState;
typedef struct a3_JSONOutput a3_JSONOutput;

enum a3_JSONTokenType {
    A3_JSONTOK_NONE,
    A3_JSONTOK_CHAR,
    A3_JSONTOK_NUM,
    A3_JSONTOK_STRING,
    A3_JSONTOK_TRUE,
    A3_JSONTOK_FALSE,
    A3_JSONTOK_NULL,
};

struct a3_JSONToken {
    a3_JSONTokenType type;

    union {
        double token_num;
        const char* token_string;
        char token_char;
    };
};

struct a3_JSONParseState {
    a3ui32 token_count;
    a3_JSONToken* tokens;
    a3_JSONToken* tokens_cur;
    a3_JSONToken* tokens_end;


    char** keys;
    a3ui32 keys_count;
    a3ui32 key_capacity;

    a3_JSONValue* values;
    a3ui32 values_count;
    a3ui32 values_capacity;


};




struct a3_JSONParse {
    a3_JSONValue top_value;
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
a3_JSONValue a3readJSONFromFile(const char* path)
{
    char* buf;
    a3ReadFileIntoMemory(path, &buf);


    a3_JSONValue val = a3readJSONFromString(buf);
    free(buf);
    return val;
}

void json_lex_next(a3_JSONLexState* state);
a3_JSONValue json_parse(a3_JSONLexState* lex_state);
void print_json_structure(a3_JSONValue val, a3ui8 depth, a3boolean isobjval);

a3_JSONValue a3readJSONFromString(const char* buffer)
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

    a3_JSONValue val = json_parse(state);

    print_json_structure(val, 0, false);
    return val;
}


/*
Parser

this sucks cause its recursive
*/

a3_JSONValue json_parse_value(a3_JSONParseState* state);

a3_JSONValue json_parse_object(a3_JSONParseState* state) {

    a3_JSONToken* last_tok = state->tokens_cur - 1;
    if (last_tok->type != A3_JSONTOK_CHAR || last_tok->token_char != '{')
        printf("error: object did not start with {");

    // empty object
    if (state->tokens_cur->type == A3_JSONTOK_CHAR && state->tokens_cur->token_char == '}') {
        state->tokens_cur++;
        return (a3_JSONValue) { .type = JSONTYPE_OBJ };
    }

    a3_JSONValue tmp_values[50];
    const char* tmp_keys[50];
    a3ui32 index = 0;
    
    while (state->tokens_cur->type != A3_JSONTOK_NONE ) {

        if (state->tokens_cur->type != A3_JSONTOK_STRING)
            printf("error: expected key\n");

        tmp_keys[index] = state->tokens_cur->token_string;

        state->tokens_cur++;

        if (state->tokens_cur->type != A3_JSONTOK_CHAR || state->tokens_cur->token_char != ':')
            printf("error: expected colon \n");

        state->tokens_cur++;

        tmp_values[index] = json_parse_value(state);

        index++;

        if (state->tokens_cur->type != A3_JSONTOK_CHAR) 
            printf("error: expected character\n");

        if (state->tokens_cur->token_char == '}') {
            state->tokens_cur++;
            break;
        }
        else if (state->tokens_cur->token_char != ',') {
            printf("error: expected end of object or comma\n");
        }

       
       
        state->tokens_cur++;
        if (index >= 50) {
            printf("error: too many items in object");
        }
    }

    
    a3ui32 keys_size = index * sizeof(char*);
    a3ui32 values_size = index * sizeof(a3_JSONValue);
    a3ui8* buf = (a3ui8*)malloc(keys_size + values_size);
    char** keys = (char** )buf;
    a3_JSONValue* values = (a3_JSONValue*)(buf + keys_size);

    memcpy(keys, tmp_keys, keys_size);
    memcpy(values, tmp_values, values_size);

    a3_JSONObject obj = (a3_JSONObject) { .length=index, .keys=keys, .values=values};

    return (a3_JSONValue) { .type = JSONTYPE_OBJ, .obj=obj };
}

a3_JSONValue json_parse_array(a3_JSONParseState* state) {

    a3_JSONValue tmp_arr[200];
    a3ui32 index = 0;

    // empty array
    if (state->tokens_cur->type == A3_JSONTOK_CHAR && state->tokens_cur->token_char == ']') {
        state->tokens_cur++;
        return (a3_JSONValue) { .type = JSONTYPE_ARRAY };
    }

    while (state->tokens_cur->type != A3_JSONTOK_NONE) {
        tmp_arr[index] = json_parse_value(state);
        index++;

        if (state->tokens_cur->type != A3_JSONTOK_CHAR) {
            printf("error: unexpected token in array\n");
        }

        char next = state->tokens_cur->token_char;
        state->tokens_cur++;
        if (next == ']') 
            break;
        
        if (next != ',') 
            printf("error: unexpected character in array\n");
        if (index >= 200) {
            printf("error: too many items in array");
        }
    }

    

    a3_JSONValue* buf = (a3_JSONValue*)malloc(index * sizeof(a3_JSONValue));
    memcpy(buf, tmp_arr, index * sizeof(a3_JSONValue));

    a3_JSONArray arr = (a3_JSONArray) {.values = buf, .length=index };
    return (a3_JSONValue) { .type = JSONTYPE_ARRAY, .arr=arr };
}

a3_JSONValue json_parse_value(a3_JSONParseState* state) {

    a3_JSONToken tok = *state->tokens_cur;
    state->tokens_cur++;
    if (tok.type == A3_JSONTOK_CHAR) {

        if (tok.token_char == '{')
            return json_parse_object(state);

        if (tok.token_char == '[')
            return json_parse_array(state);
        
        
        printf("error: wrong upper level character\n");

        return (a3_JSONValue) { .type=JSONTYPE_NONE };
    }
    
    if (tok.type == A3_JSONTOK_NUM)
        return (a3_JSONValue) { .type=JSONTYPE_NUM, .num=tok.token_num };
    
    if (tok.type == A3_JSONTOK_NULL)
        return (a3_JSONValue) { .type = JSONTYPE_NULL };

    if (tok.type == A3_JSONTOK_TRUE)
        return (a3_JSONValue) { .type = JSONTYPE_TRUE };

    if (tok.type == A3_JSONTOK_FALSE)
        return (a3_JSONValue) { .type = JSONTYPE_FALSE };

    if (tok.type == A3_JSONTOK_STRING)
        return (a3_JSONValue) { .type = JSONTYPE_STR, .str= tok.token_string };
   
    // error 
    return (a3_JSONValue) { .type = JSONTYPE_NONE };
}

a3_JSONValue json_parse(a3_JSONLexState* lex_state) {

    a3_JSONParseState state[1];
    state->tokens = lex_state->tokens;
    state->token_count = lex_state->count;
    state->tokens_cur = lex_state->tokens;
    state->tokens_end = state->tokens + state->token_count;
    
    a3_JSONValue value = json_parse_value(state);

    return value;
}


void print_padding(a3ui8 pad) {
    for (a3ui8 i = 0; i < pad; i++) printf("    ");
}

void print_json_structure(a3_JSONValue val, a3ui8 depth, a3boolean isobjval) {
    if (!isobjval)
        print_padding(depth);
    if (val.type == JSONTYPE_ARRAY) {
        
        printf("[\n");

        for (a3ui32 i = 0; i < val.arr.length; i++) {
            print_json_structure(val.arr.values[i], depth+1, false);
        }
        print_padding(depth);
        printf("]\n");

        return;
    }

    if (val.type == JSONTYPE_OBJ) {

        printf("{\n");

        for (a3ui32 i = 0; i < val.obj.length; i++) {
            print_padding(depth+1);
            printf("%s: ", val.obj.keys[i]);
            print_json_structure(val.obj.values[i], depth+1, true);
        }
        print_padding(depth);
        printf("}\n");
        
        return;
    }
    
    if (val.type == JSONTYPE_STR) {
       printf("%s\n", val.str);

        return;
    }

    if (val.type == JSONTYPE_NUM) {
        printf("%lf\n", val.num);

        return;
    }

    if (val.type == JSONTYPE_FALSE) {
        printf("false\n");

        return;
    }

    if (val.type == JSONTYPE_TRUE) {
        printf("true\n");

        return;
    }

    if (val.type == JSONTYPE_NULL) {
        printf("null\n");

        return;
    }
}


/*
Lexatizer:
break the string input into the parts
*/

static char tmp_string[1024 * 1024];




void json_lex_string(a3_JSONLexState* state);
void json_lex_num(a3_JSONLexState* state);
void json_lex_char(a3_JSONLexState* state);
void json_lex_append(a3_JSONLexState* state, a3_JSONToken token);
void json_lex_literal(a3_JSONLexState* state, a3_JSONTokenType type, const char* text);

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
    case '\t':
    case '\n':
    case '\r':
    case ' ':
        state->ptr++; break;
    case 't':
        json_lex_literal(state, A3_JSONTOK_TRUE, "true"); break;
    case 'n':
        json_lex_literal(state, A3_JSONTOK_NULL, "null"); break;
    case 'f':
        json_lex_literal(state, A3_JSONTOK_FALSE, "false"); break;
    default:
        printf("unexpected character: %c \n", *state->ptr);
       // error
        break;
    }
}


void json_lex_print_tok(a3_JSONToken tok) {
    switch (tok.type) {
    case A3_JSONTOK_STRING:
        printf("STRING: \"%s\" \n", tok.token_string);
        break;
    case A3_JSONTOK_CHAR:
        printf("CHAR: %c\n", tok.token_char);
        break;
    case A3_JSONTOK_NUM:
        printf("NUM: %lf\n", tok.token_num);
        break;
    case A3_JSONTOK_NONE:
        puts("EMPTY_TOKEN");
        break;
    default:
        puts("UNKNOWN");
        break;
    }
}


void json_lex_append_str(a3_JSONLexState* state, char* str_start, a3ui32 size) {
    // TODO: page based allocator
    char* str = malloc(size);
    memcpy(str, str_start, size);

    json_lex_append(state, (a3_JSONToken) { .type = A3_JSONTOK_STRING, .token_string = str });
}

void json_lex_append_num(a3_JSONLexState* state, double num) {
    json_lex_append(state, (a3_JSONToken) { .type = A3_JSONTOK_NUM, .token_num = num });
}

void json_lex_literal(a3_JSONLexState* state, a3_JSONTokenType type, const char* text) {
    while (*state->ptr && *text) {
        if (*state->ptr++ != *text++) {
            printf("error reading literal \"%s\"\n", text);
        }
    }

    json_lex_append(state, (a3_JSONToken) { .type=type });
}

void json_lex_append(a3_JSONLexState* state, a3_JSONToken tok) {
    if (state->count >= state->capacity) {
        // TOOD: better growth 
        state->capacity += 10;
        a3ResizeArray(state->tokens, state->capacity, a3_JSONToken);
    }
    
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
    char c;
    do {
        c = *state->ptr;
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
       

        size++;
        write++;
        state->ptr++;

        // eof or quotation mark will be eof
    } while (c);

  


   json_lex_append_str(state, tmp_string, size);
}

void json_lex_num(a3_JSONLexState* state) {
    const char* start = state->ptr;

    double sign = 1.0;
    double exp_sign = 0;
    a3ui32 part_frac = 0;
    a3ui32 part_int = 0;
    a3ui32 part_exp = 0;
    a3ui32 frac_div = 0;


    do {
        char c = *state->ptr;

        if (c >= '0' && c <= '9') {
            a3ui32 i = c - '0';
            if (frac_div == 0)
                part_int = (part_int * 10) + i;
            else if (exp_sign == 0) {
                part_frac = (part_frac * 10) + i;
                frac_div *= 10;
            }
            else {
                part_exp = (part_exp * 10) + i;
            }
        }
        else if (c == '-') sign = -1.0;
        else if (c == '.') frac_div = 1;
        else if (c == 'e' || c == 'E') {

            char char_sign = *++state->ptr;
            if (char_sign == '-')
                exp_sign = -1.0f;
            else if (char_sign == '+')
                exp_sign = 1.0f;
            else
                printf("error: unable to parse number\n");
                // error
        }
        else break;
    } while (*state->ptr++ != 0);


    double num = (double)part_int;
    if (frac_div != 0)
        num += (double)part_frac / (double)frac_div;

    num *= sign;

    if (exp_sign != 0) {
        double exp = pow(10, exp_sign * (double)part_exp);
        num *= exp;
    }
    json_lex_append_num(state, num);
    
}
