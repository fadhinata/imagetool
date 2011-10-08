
union dbl_64 {
  long long i64;
  double d64;
};
 
static double machine_epsilon(double value)
{
  union dbl_64 s;
  s.d64 = value;
  s.i64 ++;
  return value - s.d64;
}

void calculate_machine_epsilon(void)
{
  double machEps = 1.0;
 
  printf("current Epsilon, 1 + current Epsilon\n");
  do {
    printf("%G\t%.20lf\n", machEps, (1.0 + machEps));
    machEps /= 2.0;
    // If next epsilon yields 1, then break, because current
    // epsilon is the machine epsilon.
  } while ((double)(1.0 + (machEps/2.0)) != 1.0);
 
  printf("\nCalculated Machine epsilon: %G\n", machEps);
  printf("machine_epsilon: %G\n", machine_epsilon(1));
  printf("FLT_EPSILON:%G\n", FLT_EPSILON);
  printf("DBL_EPSILON:%G\n", DBL_EPSILON);
}
