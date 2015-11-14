typedef struct {
   int field1;
   int field2;
   int field3;
} INNER_T;

typedef struct {
   int b1;
   int b2;
} BUNCH_T;


typedef struct {
   int field1;
   int field2;
   INNER_T field3;
   int bunches_size;
   BUNCH_T *bunches;
   int counts_size;
   int *counts;
   int field4;
} OUTER_T;

