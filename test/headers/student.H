/**
 * Active Emissions mode
 */
typedef enum AE_MODE_T {
  AE_MODE_UNKNOWN=0,
  AE_MODE_CW=1,
  AE_MODE_PRN=2,
  AE_MODE_FM=3,
  AE_MODE_MULTIPING=4,
  AE_MODE_MULTITONE=5
} AE_MODE_T;


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
   AE_MODE_T lastName;
   ADDRESS_T address;
   int grades_size;
   GRADE_T *grades;
   string school;
} STUDENT_T;

