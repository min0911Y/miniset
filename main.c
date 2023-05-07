#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mst.h"
int main(int argc, char const *argv[])
{
#if 0
    FILE *fp = fopen("test.mst","rb");
    fseek(fp,0,SEEK_END);
    int fz = ftell(fp);
    fseek(fp,0,SEEK_SET);
    char *buf = malloc(fz+1);
    fread(buf,fz,1,fp);
    buf[fz] = 0;
    MST_Object *m =  Init_MstObj(buf);
    Var *v = MST_GetVar("hello", MST_GetRootSpace(m));
    printf("Value=%d\n",MST_Space_GetInteger(v));
    Var *v1 = MST_GetVar("test", MST_GetRootSpace(m));
    printf("Value=%s\n",MST_Space_GetStr(v1));
    Var *v2 = MST_GetVar("space", MST_GetRootSpace(m));
    SPACE *sp = MST_Space_GetSpace(v2);
    Var *v3 = MST_GetVar("val", sp);
    printf("val = %d\n",MST_Space_GetInteger(v3));
    Var *v4 = MST_GetVar("arr", MST_GetRootSpace(m));
    Array *arr = MST_Space_GetArray(v4);
    for(int i = 0;MST_Array_Get(arr, i) != NULL;i++) {
        Array_data *ad = MST_Array_Get(arr, i);
        printf("arr[%d]=%d\n",i,MST_Array_get_integer(ad));
    }
#endif
    return 0;
}
