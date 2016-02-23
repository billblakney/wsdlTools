typedef struct {
   int city;
   int state;
   int zipcode;
} ADDRESS_T;

typedef struct {
   int code;
   str level;
} COURSE_T;

typedef struct {
   string firstName;
   string lastName;
   ADDRESS_T address;
   int courses_size;
   COURSE_T *courses;
} PROF_T;

typedef struct {
   int year;
   PROF_T *profs;
   string school;
} FACULTY_T;

