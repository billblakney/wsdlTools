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
   int subject;
   int grade;
   COURSE_T course;
   int scores_size;
   int *scores;
} GRADE_T;

typedef struct {
   int firstName;
   int lastName;
   ADDRESS_T address;
   int grades_size;
   GRADE_T *grades;
   string school;
} STUDENT_T;

