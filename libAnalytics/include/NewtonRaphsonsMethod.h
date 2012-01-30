#ifndef NEWTON_RAPHSONS_METHOD_H
#define NEWTON_RAPHSONS_METHOD_H

class NewtonRaphsonsMethod {
public:
  NewtonRaphsonsMethod(/* f, f'*/);
  double findZero(
    double (*function)(double, double, double),
    double (*derivative)(double, double, double),
    double guessValue, double a, double b);
  ~NewtonRaphsonsMethod();
};

#endif // NEWTON_RAPHSONS_METHOD_H
