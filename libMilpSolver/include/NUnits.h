#ifndef N_UNITS_H
#define N_UNITS_H

#define CONSTANT_UNIT_IN_SECONDS (6)
#define VARIABLE_UNIT_IN_SECONDS (6)

#define N_CONSTANT_UNITS_PER_MIN (60 / CONSTANT_UNIT_IN_SECONDS)
#define N_VARIABLE_UNITS_PER_MIN (60 / VARIABLE_UNIT_IN_SECONDS)

#define UU_PER_MIN (N_CONSTANT_UNITS_PER_MIN)
// Since we need a shorter name for UU_PER_MIN
// U is used as a typename by boost, so avoid this
// UU is number of smallest Units Used per minute
#define VUU_PER_MIN (N_VARIABLE_UNITS_PER_MIN)

#define UU (UU_PER_MIN)
#define VUU (VUU_PER_MIN)

#define UU_PER_HR (60 * UU)
  // means number of units in hour
  // was 60, coz calculating all in minutes
  // is now changed to 600
#define VUU_PER_HR (60 * VUU)

#define UU_PER_DAY (24 * UU_PER_HR)
#define VUU_PER_DAY (24 * UU_PER_HR)

// tighter bounds can be inforced later
#define INT_VAR_LO (0 * 24 * UU_PER_HR)
#define INT_VAR_HI (1 * 24 * UU_PER_HR - 1)

/* eg:
  CONSTANT_UNIT_IN_SECONDS  UU_PER_MIN=UU  UU_PER_HR  INT_VAR_LO  INT_VAR_HI
             60                   1            60          0         1439
             30                   2           120          0         2879
             20                   3           180          0         4319
             15                   4           240          0         5759
             12                   5           300          0         7199
             10                   6           360          0         8639
              6                  10           600          0        14399
              5                  12           720          0        17279
              4                  15           900          0        21599
              3                  20          1200          0        28799
              2                  30          1800          0        43199
              1                  60          2600          0        62399
*/



// Only for TrajectScheduleGraph
#define SEN (-2)
  // sentinel


#endif // N_UNITS_H

