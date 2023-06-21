#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mst.h"
#include "mstr.h"
PUBLIC MST_API Var MST_var_make_integer(char* name, int val) {}
PUBLIC MST_API Var MST_var_make_string(char* name, char* String) {}

PUBLIC MST_API Array_data MST_arr_dat_make_integer(int val) {}
PUBLIC MST_API Array_data MST_arr_dat_make_string(char* String) {}

// TODO:我们需要将name（char *）的数据送到tokan这个链表里面，因为我们需要Free
PUBLIC MST_API void MST_add_data_to_array(MST_Object* mst_obj,
                                          Array* arr,
                                          Array_data ad) {
  if (ad.vt == SPAC || ad.vt == ARRAY) {
    mst_obj->err = WRONG_TYPE_TO_ADD;
    return;
  }
}
PUBLIC MST_API void MST_add_var_to_space(MST_Object* mst_obj,
                                         SPACE* sp,
                                         Var var) {
  if (var.vt == SPAC || var.vt == ARRAY) {
    mst_obj->err = WRONG_TYPE_TO_ADD;
    return;
  }
}
PUBLIC MST_API void MST_add_empty_space_to_space(MST_Object* mst_obj,
                                                 SPACE* sp,
                                                 char* name) {}
PUBLIC MST_API void MST_add_empty_array_to_space(MST_Object* mst_obj,
                                                 SPACE* sp,
                                                 char* name) {}
PUBLIC MST_API void MST_add_empty_space_to_array(MST_Object* mst_obj,
                                                 Array* arr) {}
PUBLIC MST_API void MST_add_empty_array_to_array(MST_Object* mst_obj,
                                                 Array* arr) {}

PRIVATE void mst_add_str(mstr* result, char* str, int space_no) {
  for (int i = 0; i < space_no; i++) {
    mstr_add_char(result, '\t');
  }
  mstr_add_str(result, str);
}
PRIVATE void build_space(mstr* result, SPACE* space, int spaces_no);
PRIVATE void build_array(mstr* result,
                         Array* arr,
                         int spaces_no_for_start,
                         int spaces_no) {
  mst_add_str(result, "[", spaces_no_for_start);
  for (int i = 0; MST_Array_Get(arr, i) != NULL; i++) {
    // printf(".\n");
    Array_data* ad = (Array_data*)MST_Array_Get(arr, i);
    switch (ad->vt) {
      case SPAC:
        mstr_add_str(result, "{\n");
        build_space(result, MST_Array_get_space(ad), spaces_no + 1);
        mst_add_str(result, "}", spaces_no);
        break;
      case INTEGER: {
        char num_buff[50];
        sprintf(num_buff, "%d", MST_Array_get_integer(ad));
        mstr_add_str(result, num_buff);
        break;
      }
      case STR: {
        char* buff = (char*)malloc(strlen(MST_Array_get_str(ad)) + 3);
        strcpy(buff, MST_Array_get_str(ad));
        sprintf(buff, "\"%s\"", MST_Array_get_str(ad));
        mstr_add_str(result, buff);
        free(buff);
        break;
      }
      case ARRAY:
        build_array(result, MST_Array_get_array(ad), 0, spaces_no);
        break;
      default:
        break;
    }
    if (MST_Array_Get(arr, i + 1) != NULL) {
      mstr_add_char(result, ',');
    }
  }
  mstr_add_char(result, ']');
}
PRIVATE void build_space(mstr* result, SPACE* space, int spaces_no) {
  for (int i = 1; FindForCount(i, space->the_space) != NULL; i++) {
    Var* sp = (Var*)FindForCount(i, space->the_space)->val;
    //  printf("name found!\n");
    char* n = (char*)malloc(strlen(sp->name) + 6);
    strcpy(n, sp->name);
    sprintf(n, "\"%s\" = ", sp->name);
    // printf("n=%s %d\n",n,i);
    mst_add_str(result, n, spaces_no);
    free(n);
    switch (sp->vt) {
      case SPAC:
        mst_add_str(result, "{\n", spaces_no);
        build_space(result, MST_Space_GetSpace(sp), spaces_no + 1);
        mst_add_str(result, "}", spaces_no);
        break;
      case INTEGER: {
        char num_buff[50];
        sprintf(num_buff, "%d", MST_Space_GetInteger(sp));
        mstr_add_str(result, num_buff);
        break;
      }
      case STR: {
        char* buff = (char*)malloc(strlen(MST_Space_GetStr(sp)) + 3);
        strcpy(buff, MST_Space_GetStr(sp));
        sprintf(buff, "\"%s\"", MST_Space_GetStr(sp));
        mstr_add_str(result, buff);
        free(buff);
        break;
      }
      case ARRAY:
        build_array(result, MST_Space_GetArray(sp), 0, spaces_no);
        break;
      default:
        break;
    }
    mstr_add_char(result, '\n');
  }
}
PUBLIC MST_API char* MST_build_to_string(MST_Object* mst_obj) {
  mstr* ms = mstr_init();
  // printf("mstr init ok!\n");
  build_space(ms, MST_GetRootSpace(mst_obj), 0);
  // printf("build_space!\n");
  char* s = (char*)malloc(strlen(mstr_get(ms)) + 1);
  strcpy(s, mstr_get(ms));
  mstr_free(ms);
  return s;
}