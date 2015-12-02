#ifndef _MEM_STRUCT_
#define _MEM_STRUCT_

struct fb{
  size_t size;
  struct fb* next;
};

struct ab{
  size_t size;
  struct ab* next;
};

#define TRUE (2==2)
#define FALSE (2==3)

#endif
