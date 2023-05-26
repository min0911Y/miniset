#include "mst.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef MEM_LEAK_CHK
int _malloc_times = 0;
int _free_times = 0;
void* Malloc(size_t size) {
  _malloc_times++;
  return malloc(size);
}
void Free(void* ptr) {
  _free_times++;
  return free(ptr);
}
#define malloc Malloc
#define free Free
#endif
/*
  put_token:
  参数：buf token字符串

*/
PRIVATE void put_token(char* buf, TOKEN_TYPE t, MST_Object* mst) {
  TOKEN* tok = malloc(sizeof(TOKEN));
  tok->t = t;
  tok->tok = buf;
  AddVal((uintptr_t)tok, mst->token);
}
PRIVATE char* __next(char* buf) {
  char *x, *y;
  x = strchr(buf, ' ');
  y = strchr(buf, '\t');
  return x < y ? (x != 0 ? x : y) : (y != 0 ? y : x);
}
PRIVATE char* _next(char* buf) {
  if (buf[0] == '#') {
    return buf;  // do nothing
  }
  char *x, *y;
  x = __next(buf);
  y = strchr(buf, '\n');
  return x < y ? (x != 0 ? x : y) : (y != 0 ? y : x);
}
PRIVATE char* next1(char* buf) {
  char* p = _next(buf);
  if (!(p))
    return 0;
  while (p[0] == '#') {
    p = strchr(p, '\n');
    if (p) {
      p++;
      if (!(*p)) {
        return 0;
      }
    } else {
      return 0;
    }
  }
  while (*p == ' ' || *p == '\t' || *p == '\n') {
    p++;
  }
  if (*p) {
    while (p[0] == '#') {
      p = strchr(p, '\n');
      if (p) {
        p++;
        if (!(*p)) {
          return 0;
        }
      } else {
        return 0;
      }
    }
    while ((*p == ' ' || *p == '\t' || *p == '\n') && *p) {
      p++;
    }
    return *p ? p : 0;
  } else
    return 0;
}
PRIVATE char* next(char* buf) {
  char* p = next1(buf);
  if (!p) {
    return 0;
  }
  char* p1 = p;
  int fg = 0;
  while ((*p != '#' && fg) && *p != ' ' && *p) {
    if (*p == '\"') {
      fg = !fg;
    }
    p++;
  }
  if (*p != '#') {
    return p1;
  } else {
    while (*p != '\n' && *p != '\0') {
      *p = ' ';
      p++;
    }
    return p1;
  }
}
PRIVATE char* get_str(char* text, char** p) {
  size_t len = 0;
  char* src = text;
  char* result;
  int flag_str = 0;
  int flag_str1 = 0;
  int flag_op = 0;
  while (*text) {
    // printf("%c\n",*text);
    len++;
    if (!flag_str && *text == '[') {
      flag_op = 1;
    }
    if (!flag_str && *text == ']') {
      flag_op = 0;
    }
    if (!flag_op) {
      if (*text == '\\') {
        flag_str1 = !flag_str1;
      } else if (flag_str1) {
        flag_str1 = 0;
      } else {
        if (*text == '\"') {
          flag_str = !flag_str;
        }
      }
    }
    if (*text == '#' || *text == ' ' || *text == '\n') {
      if (flag_str == 0 && flag_op == 0) {
        // printf("This.\n");
        break;
      }
    }
    text++;
  }
  if (flag_op + flag_str1 + flag_str != 0) {
    return NULL;
  }
  result = (char*)malloc(len + 1);
  result[len] = 0;
  memcpy(result, src, len);
  *p = text;
  return result;
}
PRIVATE size_t token_strlen(char* s) {
  size_t result = 0;
  if (*s != '\"') {
    return 0;
  }
  s++;
  while (*s != '\"' && *s) {
    result++;
    s++;
  }
  return result;
}
PRIVATE int token_put_string(char* p1, MST_Object* mst) {
  char* result;
  result = malloc(token_strlen(p1) + 1);
  TOKEN_TYPE tt;
  size_t length = strlen(p1);
  int flag = 0;
  if (p1[0] == '\"') {
    tt = STRING;
    int j = 0;
    for (int i = 1; i < length; i++) {
      if (p1[i] == '\"' && !flag) {
        break;
      }
      if (flag) {
        flag = 0;
      }
      if (p1[i] == '\\') {
        flag = 1;
      }
      result[j++] = p1[i];
    }
    result[j] = 0;
    put_token(result, tt, mst);
    return j + 1;
  } else {
    free(result);
    return 0;
  }
}
PRIVATE int token_put_integer(char* p1, MST_Object* mst) {
  char* result;
  result = malloc(strlen(p1) + 1);
  size_t length = strlen(p1);
  int j = 0;
  if (p1[0] == '-') {
    result[j++] = p1[0];
    p1++;
    length--;
  }
  for (int i = 0; i < length; i++) {
    if (isdigit(p1[i])) {
      result[j++] = p1[i];
    } else {
      break;
    }
  }
  result[j] = 0;
  put_token(result, NUMBER, mst);
  return j - 1;
}
PRIVATE void auto_put_token(char* p1, MST_Object* mst) {
  // printf("p1=%s\n",p1);

  size_t length = strlen(p1);

  for (int i = 0; i < length; i++) {
    switch (p1[i]) {
      case '\"':
        //   printf("str.\n");
        i = i + token_put_string(p1 + i, mst);
        break;
      case '[': {
        char* r = malloc(2);
        r[1] = 0;
        r[0] = '[';
        put_token(r, ARRAY_START, mst);
        break;
      }
      case ']': {
        char* r = malloc(2);
        r[1] = 0;
        r[0] = ']';
        put_token(r, ARRAY_END, mst);
        break;
      }
      case '{': {
        char* r = malloc(2);
        r[1] = 0;
        r[0] = '{';
        put_token(r, SPACE_START, mst);
        break;
      }
      case '}': {
        char* r = malloc(2);
        r[1] = 0;
        r[0] = '}';
        put_token(r, SPACE_END, mst);
        break;
      }
      case '=': {
        char* r = malloc(2);
        r[1] = 0;
        r[0] = '=';
        put_token(r, OP, mst);
        break;
      }
      case ' ':
      case '\r':
      case '\n':
      case ',':
        break;
      default: {
        if (isdigit(p1[i]) || p1[i] == '-') {
          i = i + token_put_integer(p1 + i, mst);
        } else {
          mst->err = UNKNOW_TOKEN;
          return;
        }
        break;
      }
    }
  }
}
PRIVATE void Mst_lexer(char* buffer, MST_Object* mst) {
  char* start = buffer;
  char* p = start;
  while (p) {
    if (p[0] != '#') {
      // 处理token
      // printf("------\n%s-----\n",p);
      char* p1 = get_str(p, &p);
      if (!p1) {
        mst->err = STRING_OP_ERROR;
        return;
      }
      auto_put_token(p1, mst);
      free(p1);
      if (mst->err) {
        return;
      }
    }
    p = next(p);  // next
  }
}
PRIVATE void free_arr(Array* arr);
PRIVATE void free_space(SPACE* space);
PRIVATE void process_token(MST_Object* mobj) {
  TOKEN* t_bmp[2];
  int idx = 0;
  for (int i = 1; FindForCount(i, mobj->token) != NULL; i++) {
    TOKEN* t = (TOKEN*)FindForCount(i, mobj->token)->val;
    t_bmp[idx] = t;
    if (idx) {
      if (t_bmp[0]->t == STRING &&
          (t_bmp[1]->t == OP && t_bmp[1]->tok[0] == '=')) {
        t_bmp[0]->t = WORD;
      } else if (t_bmp[1]->t == STRING) {
        t_bmp[0] = t_bmp[1];
        idx = 0;
      }
    }
    idx = !idx;
  }
}
PRIVATE int parser_space(MST_Object* mst, int idx, SPACE* space, int mode);
PRIVATE int parser_array(MST_Object* mst, int idx, Array* arr) {
  int i = idx;
  for (; FindForCount(i, mst->token) != NULL; i++) {
    TOKEN* t = (TOKEN*)FindForCount(i, mst->token)->val;
    if (t->t == ARRAY_END) {
      break;
    }
    if (t->t == SPACE_START) {
      //     printf("Array:Found a SPACE\n");
      Array_data* v = (Array_data*)malloc(sizeof(Array_data));
      v->vt = SPAC;
      v->obj = (SPACE*)malloc(sizeof(Array));
      SPACE* sp = (SPACE*)v->obj;
      sp->the_space = NewList();
      i = parser_space(mst, i + 1, sp, 0);
      if (i == -1) {
        free_space(sp);
        DeleteList(sp->the_space);
        free(v->obj);
        free(v);
        return -1;
      }
      AddVal((uintptr_t)v, arr->the_array);
    } else if (t->t == ARRAY_START) {
      //  printf("Array:Found a Array\n");
      Array_data* v = (Array_data*)malloc(sizeof(Array_data));
      v->vt = ARRAY;
      v->obj = (Array*)malloc(sizeof(Array));
      Array* arr1 = (Array*)v->obj;
      arr1->the_array = NewList();
      i = parser_array(mst, i + 1, arr1);
      if (i == -1) {
        free_arr(arr1);
        DeleteList(arr1->the_array);
        free(v->obj);
        free(v);
        return -1;
      }
      AddVal((uintptr_t)v, arr->the_array);
    } else if (t->t == INTEGER) {
      //  printf("Array:Found a Number:%s\n",t->tok);
      Array_data* v = (Array_data*)malloc(sizeof(Array_data));
      v->vt = INTEGER;
      v->obj = (Integer*)malloc(sizeof(Integer));
      Integer* number = (Integer*)v->obj;
      number->num = strtol(t->tok, NULL, 10);
      AddVal((uintptr_t)v, arr->the_array);
    } else if (t->t == STRING) {
      //   printf("Array:Found a String:%s\n",t->tok);
      Array_data* v = (Array_data*)malloc(sizeof(Array_data));
      v->vt = STR;
      v->obj = (String*)malloc(sizeof(String));
      String* str = (String*)v->obj;
      str->str = t->tok;
      AddVal((uintptr_t)v, arr->the_array);
    } else {
      mst->err = ERROR_TOKEN;
      return -1;
    }
  }
  return i;
}
PRIVATE void free_space(SPACE* space);
PRIVATE void free_arr(Array* arr) {
  int i = 1;
  for (; FindForCount(i, arr->the_array) != NULL; i++) {
    Array_data* v = (Array_data*)FindForCount(i, arr->the_array)->val;
    switch (v->vt) {
      case INTEGER:
        free(v->obj);
        break;
      case STR:
        free(v->obj);
        break;
      case SPAC:
        free_space((SPACE*)v->obj);
        DeleteList(((SPACE*)v->obj)->the_space);
        free(v->obj);
        break;
      case ARRAY:
        free_arr((Array*)v->obj);
        DeleteList(((Array*)v->obj)->the_array);
        free(v->obj);
        break;
      default:
        break;
    }
    free(v);
  }
}
PRIVATE void free_space(SPACE* space) {
  int i = 1;
  if (!space) {
    return;
  }
  for (; FindForCount(i, space->the_space) != NULL; i++) {
    Var* v = (Var*)FindForCount(i, space->the_space)->val;
    switch (v->vt) {
      case INTEGER:
        free(v->obj);
        break;
      case STR:
        free(v->obj);
        break;
      case SPAC:
        free_space((SPACE*)v->obj);
        DeleteList(((SPACE*)v->obj)->the_space);
        free(v->obj);
        break;
      case ARRAY:
        free_arr((Array*)v->obj);
        DeleteList(((Array*)v->obj)->the_array);
        free(v->obj);
        break;
      default:
        break;
    }
    free(v);
  }
}
PRIVATE int parser_space(MST_Object* mst, int idx, SPACE* space, int mode) {
  int i = idx;
  int flag = 0;
  for (; FindForCount(i, mst->token) != NULL; i++) {
    TOKEN* t = (TOKEN*)FindForCount(i, mst->token)->val;
    if (!mode && t->t == SPACE_END) {
      flag = 1;
      break;
    }
    if (t->t == WORD) {
      Var* v = (Var*)malloc(sizeof(Var));
      v->name = t->tok;
      //      printf("v->name = %ll16x\n",v->name);
      TOKEN* t1 = (TOKEN*)FindForCount(i + 2, mst->token)->val;
      if (t1->t == SPACE_START) {
        //printf("START\n");
        //   printf("Found a Space %s\n",t->tok);
        v->vt = SPAC;
        v->obj = (SPACE*)malloc(sizeof(SPACE));
        SPACE* sp = (SPACE*)v->obj;
        sp->the_space = NewList();
        i = parser_space(mst, i + 3, sp, 0);
        if (i == -1) {
          free_space(sp);
          DeleteList(sp->the_space);
          free(v->obj);
          free(v);
          return -1;
        }
        AddVal((uintptr_t)v, space->the_space);
        //i += 2;
      } else if (t1->t == ARRAY_START) {
        //    printf("Found a Array:%s\n",t->tok);
        v->vt = ARRAY;
        v->obj = (Array*)malloc(sizeof(Array));
        Array* arr = (Array*)v->obj;
        arr->the_array = NewList();
        i = parser_array(mst, i + 3, arr);
        if (i == -1) {
          free_arr(arr);
          DeleteList(arr->the_array);
          free(v->obj);
          free(v);
          return -1;
        }
        AddVal((uintptr_t)v, space->the_space);
      } else if (t1->t == INTEGER) {
        //  printf("Space:Found a Number:%s\n",t->tok);
        // Array_data *v = (Array_data *)malloc(sizeof(Array_data));
        v->vt = INTEGER;
        v->obj = (Integer*)malloc(sizeof(Integer));
        Integer* number = (Integer*)v->obj;
        number->num = strtol(t1->tok, NULL, 10);
        AddVal((uintptr_t)v, space->the_space);
        i += 2;
      } else if (t1->t == STRING) {
        // printf("Space:Found a String:%s\n",t->tok);
        //  Array_data *v = (Array_data *)malloc(sizeof(Array_data));
        v->vt = STR;
        v->obj = (String*)malloc(sizeof(String));
        String* str = (String*)v->obj;

        str->str = t1->tok;
      //  printf("string = %llx %llx\n", str->str, t1->tok);
        AddVal((uintptr_t)v, space->the_space);
        i += 2;
      }
    }
  }
  if (!flag && space != MST_GetRootSpace(mst)) {
    // printf("fatal error: Couldn't find the end of opcode\n");
    // exit(-1);
    mst->err = SYNTAX_ERROR;
    return -1;
  }
  return i;
}
PUBLIC MST_API MST_Object* Init_MstObj(char* string) {
  MST_Object* result = (MST_Object*)malloc(sizeof(MST_Object));
  result->string = malloc(strlen(string) + 1);
  result->err = 0;
  result->root_space = NULL;
  strcpy(result->string, string);
  result->token = NewList();
  Mst_lexer(result->string, result);
  if (!result->err) {
    process_token(result);
    result->root_space = (SPACE*)malloc(sizeof(SPACE));
    result->root_space->the_space = NewList();
    // printf("%08x\n",result->root_space->the_space);
    parser_space(result, 1, result->root_space, 1);
  }
  return result;
}
PUBLIC MST_API Var* MST_GetVar(char* name, SPACE* space) {
  for (int i = 1; FindForCount(i, space->the_space) != NULL; i++) {
    Var* sp = (Var*)FindForCount(i, space->the_space)->val;
    if (strcmp(name, sp->name) == 0) {
      return sp;
    }
  }
  return NULL;
}
PUBLIC MST_API int MST_Space_GetInteger(Var* var) {
  if (var->vt != INTEGER) {
    return -1;
  }
  Integer* n = (Integer*)var->obj;
  return n->num;
}
PUBLIC MST_API SPACE* MST_Space_GetSpace(Var* var) {
  if (var->vt != SPAC) {
    return NULL;
  }
  SPACE* n = (SPACE*)var->obj;
  return n;
}
PUBLIC MST_API Array* MST_Space_GetArray(Var* var) {
  if (var->vt != ARRAY) {
    return NULL;
  }
  Array* n = (Array*)var->obj;
  return n;
}
PUBLIC MST_API char* MST_Space_GetStr(Var* var) {
  if (var->vt != STR) {
    return NULL;
  }
  String* n = (String*)var->obj;
  return n->str;
}
PUBLIC MST_API Array_data* MST_Array_Get(Array* arr, int idx) {
  if (FindForCount(idx + 1, arr->the_array) == NULL) {
    return NULL;
  }
  return (Array_data*)FindForCount(idx + 1, arr->the_array)->val;
}
PUBLIC MST_API int MST_Array_get_integer(Array_data* ad) {
  if (ad->vt != INTEGER) {
    return -1;
  }
  Integer* n = (Integer*)ad->obj;
  return n->num;
}
PUBLIC MST_API char* MST_Array_get_str(Array_data* ad) {
  if (ad->vt != STR) {
    return NULL;
  }
  String* n = (String*)ad->obj;
  return n->str;
}
PUBLIC MST_API SPACE* MST_Array_get_space(Array_data* ad) {
  if (ad->vt != SPAC) {
    return NULL;
  }
  SPACE* n = (SPACE*)ad->obj;
  return n;
}
PUBLIC MST_API Array* MST_Array_get_array(Array_data* ad) {
  if (ad->vt != ARRAY) {
    return NULL;
  }
  Array* n = (Array*)ad->obj;
  return n;
}
PUBLIC MST_API void MST_FreeObj(MST_Object* mst) {
  free(mst->string);
  for (int i = 1; FindForCount(i, mst->token) != NULL; i++) {
    TOKEN* t = (TOKEN*)FindForCount(i, mst->token)->val;
    //   printf("free %llx\n",t->tok);
    free(t->tok);
    free(t);
  }
  DeleteList(mst->token);

  if (mst->root_space) {
    free_space(mst->root_space);
    DeleteList(mst->root_space->the_space);
    free(mst->root_space);
  }
  free(mst);
#ifdef MEM_LEAK_CHK
  printf("malloc times:%d\nfree times:%d\n", _malloc_times, _free_times);
#endif
}
PUBLIC MST_API const char* MST_strerror(MST_Object* mst) {
  switch (mst->err) {
    case STRING_OP_ERROR:
      return "fatal error: string not closed or operator error";
      break;
    case UNKNOW_TOKEN:
      return "fatal error: An unknown token was detected.";
      break;
    case ERROR_TOKEN:
      return "fatal error: The token contained an error.";
      break;
    case SYNTAX_ERROR:
      return "fatal error: A syntax error has occurred.";
      break;
    default:
      return "(null)";
      break;
  }
}
