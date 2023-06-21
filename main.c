#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mst.h"
int main(int argc, char const* argv[]) {
#if 0
  FILE* fp = fopen("test.mst", "rb");
  fseek(fp, 0, SEEK_END);
  int fz = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  char* buf = malloc(fz + 1);
  fread(buf, fz, 1, fp);
  buf[fz] = 0;
  MST_Object* m = Init_MstObj(buf);
  if (m->err) {
    printf("%s\n", MST_strerror(m));
    MST_FreeObj(m);
    exit(-1);
  }
  Var* v = MST_GetVar("hello", MST_GetRootSpace(m));
  printf("Value=%d\n", MST_Space_GetInteger(v));
  Var* v1 = MST_GetVar("test", MST_GetRootSpace(m));
  printf("Value=%s\n", MST_Space_GetStr(v1));
  Var* v2 = MST_GetVar("space", MST_GetRootSpace(m));
  SPACE* sp = MST_Space_GetSpace(v2);
  Var* v3 = MST_GetVar("val", sp);
  printf("val = %d\n", MST_Space_GetInteger(v3));
  Var* v4 = MST_GetVar("arr", MST_GetRootSpace(m));
  Array* arr = MST_Space_GetArray(v4);
  for (int i = 0; MST_Array_Get(arr, i) != NULL; i++) {
    Array_data* ad = MST_Array_Get(arr, i);
    printf("arr[%d]=%d\n", i, MST_Array_get_integer(ad));
  }
  MST_add_var_to_space(m,MST_GetRootSpace(m),MST_var_make_integer("test_make",13123));
  MST_add_var_to_space(m,MST_GetRootSpace(m),MST_var_make_string("test_makestring","test!!!!"));
  MST_add_empty_array_to_space(m,MST_GetRootSpace(m),"test_array");
  MST_add_empty_space_to_space(m,MST_GetRootSpace(m),"test_space");
  MST_add_var_to_space(m,MST_Space_GetSpace(MST_GetVar("test_space",MST_GetRootSpace(m))),MST_var_make_integer("test_make",13123));
  MST_add_var_to_space(m,MST_Space_GetSpace(MST_GetVar("test_space",MST_GetRootSpace(m))),MST_var_make_string("test_makestring","test!!!!"));
  MST_add_data_to_array(m,MST_Space_GetArray(MST_GetVar("test_array",MST_GetRootSpace(m))),MST_arr_dat_make_integer(13123));
  MST_add_data_to_array(m,MST_Space_GetArray(MST_GetVar("test_array",MST_GetRootSpace(m))),MST_arr_dat_make_string("test!!!!"));
  char *s1 = MST_build_to_string(m);
  printf("%s\n",s1);
  free(s1);
  MST_FreeObj(m);
#endif
  return 0;
}
