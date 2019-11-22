//
// Created by serik1987 on 21.11.2019.
//

#include "ExplicitRungeKutta.h"
#include "../log/output.h"


namespace method{

    namespace ButcherTables{

        /* The following designations were used for Butcher tables

         0
         ALPHA[ALPHA_POS]       BETA[BETA_POS]
         ALPHA[ALPHA_POS+1]     BETA[BETA_POS+1]    BETA[BETA_POS+2]
         ALPHA[ALPHA_POS+2]     BETA[BETA_POS+3]    BETA[BETA_POS+4]    BETA[BETA_POS+5]
         ...........................
         ALPHA[ALPHA_POS+N-2]   BETA[BETA_POS+...]  BETA[BETA_POS+...]  ...
         ---------------------------------------------------------------------------------------
                                GAMMA[GAMMA_POS]    GAMMA[GAMMA_POS+1]  ...

        ALPHA, BETA, GAMMA are 1D arrays given below. You are free to fill them with appropriate values in order
         to develop higher order Runge Kutta methods

        ALPHA_POS, BETA_POS, GAMMA_POS are private static constant members of an appropriate method class.
        Their values are derived from the template parameter N during the compilation

        */

        /* In order to define new Runge Kutta method please, add the row to the following array */

        /* Array of alpha coefficients. Coefficients for Nth order method shall be placed strictly between coefficients
         * for N-1-th methods and coefficients for N+1-th method. N-1 coefficients are allowed to add */
        static const double ALPHA[] = {
                /* Data for Runge Kutta 1 */
                /* NO DATA */

                /* Data for Runge Kutta 2 */
                1.0,

                /* Data for Runge Kutta 3 */
                0.5,
                1.0
        };

        /* Array of alpha coefficients. Coefficients for Nth order method shall be placed strictly between coefficients
         * for N-1-th methods and coefficients for N+1-th method. N * (N-1) / 2 coefficients are allowed to add */
        static const double BETA[] = {
                /* Data for Runge Kutta 1 */
                /* No data */

                /* Data for Runge Kutta 2 */
                1.0,

                /* Data for Runge Kutta 3 */
                0.5,
                0.0,    1.0
        };

        /* Array of alpha coefficients. Coefficients for Nth order method shall be placed strictly between coefficients
         * for N-1-th methods and coefficients for N+1-th method. N coefficients are allowed to add */
        static const double GAMMA[] = {
                /* Data for Runge Kutta 1 */    1.0,
                /* Data for Runge Kutta 2 */    0.5,    0.5,
                /* Data for Runge Kutta 3 */    1.0/6,  2.0/3,  1.0/6
        };
    }

    /*
     * After any Butcher table is defined, the last step of implementation of Nth order Runge Kutta method is to tell
     * the compiler to instantiate explicitly the Nth Runge Kutta method and compile this. The best way to do this is
     * to add the line below following an example. Number in <...> is the method order.
     */
    template class ExplicitRungeKutta<1>;
    template class ExplicitRungeKutta<2>;
    template class ExplicitRungeKutta<3>;

    /*
     * Also, if you are not sure whether you changed the ALPHA, BETA and GAMMA constants correctly, uncomment the
     * following line to ensure which coefficients are actually applied
     */
// #define RUNGE_KUTTA_BEBUG





    /* There is no necessity to change the code below */


    template<int N>
    equ::Ode::SolutionParameters ExplicitRungeKutta<N>::getSolutionParameters() {
        equ::Ode::SolutionParameters parameters(getIntegrationTime(), N);
        return parameters;
    }

    template<int N>
    void ExplicitRungeKutta<N>::initialize(equ::Ode &ode) {
        ode.copy(N, 0);
    }

    template<int N>
    void ExplicitRungeKutta<N>::update(equ::Ode &ode, unsigned long long timestamp) {
        int alpha_pos = ALPHA_POS;
        int beta_pos = BETA_POS;
        int gamma_pos = GAMMA_POS;

        auto t = (double)timestamp;
        auto real_t = t * getIntegrationTime();
        ode.swap(N, 0);
        ode.calculateDerivative(0, 0, t);
#ifdef RUNGE_KUTTA_BEBUG
        logging::enter();
        logging::debug("Application of Butcher table. gamma = " + std::to_string(ButcherTables::GAMMA[gamma_pos]));
        logging::exit();
#endif
        ode.increment(N, 0, 0, ButcherTables::GAMMA[gamma_pos++]);
        for (int j=1; j < N; ++j){
#ifdef RUNGE_KUTTA_BEBUG
            logging::enter();
            logging::debug("Application of Butcher table. beta = " + std::to_string(ButcherTables::BETA[beta_pos]));
            logging::exit();
#endif
            ode.increment(j, 0, 0, ButcherTables::BETA[beta_pos++]);
            for (int i=1; i < j; ++i){
#ifdef RUNGE_KUTTA_BEBUG
                logging::enter();
                logging::debug("Application of Butcher table. beta = " + std::to_string(ButcherTables::BETA[beta_pos]));
                logging::exit();
#endif
                ode.increment(j, j, i, ButcherTables::BETA[beta_pos++]);
            }
#ifdef RUNGE_KUTTA_BEBUG
            logging::enter();
            logging::debug("Application of Butcher table. alpha = " + std::to_string(ButcherTables::ALPHA[alpha_pos]));
            logging::exit();
#endif
            ode.calculateDerivative(j, j, t + ButcherTables::ALPHA[alpha_pos++]);
#ifdef RUNGE_KUTTA_BEBUG
            logging::enter();
            logging::debug("Application of Butcher table. gamma = " + std::to_string(ButcherTables::GAMMA[gamma_pos]));
            logging::exit();
#endif
            ode.increment(N, N, j, ButcherTables::GAMMA[gamma_pos++]);
        }
        ode.update(real_t);
    }


}