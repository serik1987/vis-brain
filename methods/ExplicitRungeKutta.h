//
// Created by serik1987 on 21.11.2019.
//

#ifndef MPI2_EXPLICITRUNGEKUTTA_H
#define MPI2_EXPLICITRUNGEKUTTA_H

#include "Method.h"

namespace method {

    /**
     * Represents a set of Runge-Kutta methods
     *
     * @tparam N method order
     */
    template <int N> class ExplicitRungeKutta: public Method {
    private:
        /* Location of the alpha coefficient within the ButcherTables::ALPHA array
         * 1st order method contains 0 coefficients
         * 2nd order method contains 1 coefficient located at position 0 = (1-1) * (1-2) / 2
         * 3rd order method contains 2 coefficients located at position 1 = (3-2) * (3-1) / 2
         * ...
         * nth order method contains N-1 coefficients, after N-2 coefficients for n-1-th method, which in turn
         * are located after N-3 coefficients for n-2-th method ...
         * Its position is (N-2) + (N-3) + ... + 1 + 0 = (N-1) * (N-2) / 2 = ALPHA_POS
         */
        static constexpr int ALPHA_POS = (N-1) * (N-2) / 2;


        /* Location of the beta coefficient within the BETA 1D array
         * 1st order has 0 coefficients
         * 2nd order has 1 coefficient starting from the position 0
         * 3rd order has 3 coefficients starting from the position: 0 + 1 (0 = initial position, 1 = number of
         * coefficients for the second order.
         * Analogously, 4th order has 6 coefficients starting from the position 0 + 1 + 3
         * 5th order has 10 coefficients starting from the position 0 + 1 + 3 + 6
         * 6th order starts from the position 0 + 1 + 3 + 6 + 10
         *
         * Let's suppose d_N = N*(N-1)/2 - total number of the beta coefficients in the Butcher table
         * Position of the 1st coefficient for 2nd order method is 0
         * Position of the 1st coefficient for 3rd order method is d_2 = 2 * 1 / 2 = 1
         * Position of the 1st coefficient for 4th order method is d_2 + d_3
         * .... for Nth order the value is sum_{k=2}^{N-1}d_k = sum_{k=0}^{N-1}d_k because d_1 = d_0 = 0
         *
         * Hence, we have:
         * Statement # 1. Coefficients for Nth order method in the BETA array equal to:
         * BETA_POS{N} = sum_{k=0}^{N-1}{k*(k-1)/2}                     (Eq. 1)
         *
         * Next, as an easy exercise for memorizing your Secondary Education, prove the following statements using the
         * Eq. 1
         *
         * Statement # 2. If N is even, BETA_POS{N} equals to the sum of squares of all even numbers from 2 to N-2:
         * BETA_POS{N} = 2^2 + 4^2 + 6^2 + ... + (N-2)^2                (Eq. 2)
         *
         * Statement # 3. If N is odd, BETA_POS{N} equals to the sum of squares of all odd numbers from 2 to N-2:
         * BETA_POS{N} = 1^2 + 3^2 + 5^2 + ... + (N-2)^2                (Eq. 3)
         *
         * Statement # 4. Sum of squares of the 1st n odd numbers is:
         * 1^2 + 3^2 + ... + (2n-1)^2 = n(2n-1)(2n+1)/3                 (Eq. 4)
         *
         * Statement # 5. Sum of squares of the 1st n even numbers is:
         * 2^2 + 4^2 + ... + (2n)^2 = 2n(n+1)(2n+1)/3                   (Eq. 5)
         *
         * Substituting (Eq. 2) into (Eq. 5), we obtain that for even N's:
         * BETA_POS{N} = N * (N-1) * (N-2) / 6
         *
         * After substitution of (Eq. 3) into (Eq. 4) we obtain the same result for odd N's.
         */
        static constexpr int BETA_POS = N * (N-1) * (N-2) / 6;

        /* Location of the gamma coefficient
         * 1st order method contains 1 coefficient located at position 0
         * 2nd order method contains 2 coefficients located at position 1 = 0 + 1
         * 3rd order method contains 3 coefficients located at position 3 = 0 + 1 + 2
         * 4th order method contains 4 coefficients located at position 6 = 0 + 1 + 2 + 3
         * ...
         * nth order method contains N coefficients, located at position 0 + 1 + 2 + ... + N-1 = N * (N-1) / 2
         */
        static constexpr int GAMMA_POS = N * (N-1) / 2;

    public:
        explicit ExplicitRungeKutta(double dt): Method(dt) {};

        equ::Ode::SolutionParameters getSolutionParameters() override;

        void initialize(equ::Ode& ode) override;
        void update(equ::Ode& ode, unsigned long long timestamp) override;
    };

    template<int N> void GET_EXPLICIT_RUNGE_KUTTA(int n, double dt, Method** pmethod){
        if (n == N){
            *pmethod = new ExplicitRungeKutta<N>(dt);
        }
    }

}


#endif //MPI2_EXPLICITRUNGEKUTTA_H
