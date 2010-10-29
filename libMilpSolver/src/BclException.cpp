#include "BclException.h"

#ifdef USE_XPRESS_NATIVE

using namespace std;
using namespace ::dashoptimization;

BclException::BclException(int c,const char *m) {
  code = c;
  msg = string(m);
  cout << "EXCP:" << msg << '\n';
}


/**** User error handling function ****/
void XPRB_CC usererror(
  xbprob * prob, void * vp, int num, int type, const char * t) {
  throw BclException(num, t);
}

/**** User printing function ****/
void XPRB_CC userprint(xbprob * prob, void * vp, const char * msg) {
  static int rtsbefore=1;
  /* Print ’BCL output’ whenever a new output line starts,
  otherwise continue to print the current line. */
  if (rtsbefore) {
    cout << "BCL output: " << msg;
  } else {
    cout << msg;
  }
  rtsbefore=(msg[strlen(msg)-1]=='\n');
}

/***********************************************************************/
void modexpl3(XPRBprob &p) {
  XPRBvar x[3];
  XPRBlinExp le;
  int i;
  for (i=0;i<2;i++) {
    x[i] = p.newVar(
     XPRBnewname("x_%d",i), XPRB_UI, 0, 100);
  }
  /* Create the constraints:
  C1: 2x0 + 3x1 >= 41
  C2: x0 + 2x1 = 13 */
  p.newCtr("C1", 2*x[0] + 3*x[1] >= 41);
  p.newCtr("C2", x[0] + 2*x[1] == 13);
  // Uncomment the following line to cause an error in the model that
  // triggers the user error handling:
  // x[2]=p.newVar("x_2", XPRB_UI, 10,1);
  le=0;
  for (i=0;i<2;i++) {
    le += x[i]; // Objective: minimize x0+x1
  }
  p.setObj(le); // Select objective function
  p.setSense(XPRB_MINIM); // Set objective sense to minimization
  p.solve(""); // Solve the LP
  XPRBprintf(p.getCRef(), "problem status: %d LP status: %d MIP status: %d\n",
  p.getProbStat(), p.getLPStat(), p.getMIPStat());
  // This problem is infeasible, that means the following command will fail.
  // It prints a warning if the message level is at least 2
  XPRBprintf(p.getCRef(), "Objective: %g\n", p.getObjVal());
  for (i=0;i<2;i++) { // Print solution values
    XPRBprintf(p.getCRef(), "%s:%g, ", x[i].getName(), x[i].getSol());
  }
  XPRBprintf(p.getCRef(), "\n");
}
/***********************************************************************/

#endif // USE_XPRESS_NATIVE
