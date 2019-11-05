//
// Created by serik1987 on 04.11.2019.
//

#ifndef MPI2_DISTRIBUTIONS_H
#define MPI2_DISTRIBUTIONS_H

#include <random>
#include "NoiseEngine.h"
#include "../../Application.h"

namespace data::noise {

    /**
     * A common parent of all distributions. In order for the distribution to be substituted to the noise
     * the distribution shall be derived from this class
     */
    class Distribution {
    protected:
        NoiseEngine &engine;

    public:
        Distribution(): engine(Application::getInstance().getNoiseEngine()) {};

        /**
         * Generates random number according to a certain distribution
         */
        virtual double operator()() = 0;

        /**
         * Resets the random number
         */
        virtual void reset() = 0;
    };

    /**
     * A Facade for std::bernoulli_distribution
     * Emits a spike at a single timestamp
     */
    class BernoulliDistribution: public Distribution, private std::bernoulli_distribution{
    public:
        /**
         *
         * @param p relation of the spike rate to the sample rate
         */
        explicit BernoulliDistribution(double p = 0.5): Distribution(), std::bernoulli_distribution(p) {};

        double operator()() override{
            bool number = std::bernoulli_distribution::operator()(engine);
            if (number) return 1.0;
            else return 0.0;
        }

        void reset() override { std::bernoulli_distribution::reset(); }
    };

    /**
     * Facade for std::binomial_distribution
     * Emits certain number of spikes within N fixed timestamps
     */
    class BinomialDistribution: public Distribution, private std::binomial_distribution<int>{
    public:
        /**
         *
         * @param t number of timestamps where the spikes shall be emitted
         * @param p ratio of spike rate to the firing rate
         */
        explicit BinomialDistribution(int t = 1, double p = 0.5): Distribution(), std::binomial_distribution<int>(t, p) {};

        double operator()() override{
            int number = std::binomial_distribution<int>::operator()(engine);
            return (double)number;
        }

        void reset() override { std::binomial_distribution<int>::reset(); }
    };

    /**
     * Facade for std::cauchy_distribution
     * Generates a value that is ratio of two normally distributed values
     */
    class CauchyDistribution: public Distribution, private std::cauchy_distribution<double>{
    public:
        explicit CauchyDistribution(double a, double b): Distribution(), std::cauchy_distribution<double>(a, b) {};

        double operator()() override {
            return std::cauchy_distribution<double>::operator()(engine);
        }

        void reset() override { std::cauchy_distribution<double>::reset(); }
    };

    /**
     * Facade for std::chi_squared_distribution
     * Generates value which is a sum of squares of n normally distributed values with mean = 0, std = 1
     */
    class ChiSquaredDistribution: public Distribution, private std::chi_squared_distribution<double>{
    public:
        /**
         *
         * @param n number of these values
         */
        explicit ChiSquaredDistribution(double n): Distribution(), std::chi_squared_distribution<double>(n) {};

        double operator()() override {
            return std::chi_squared_distribution<double>::operator()(engine);
        }

        void reset() override { std::chi_squared_distribution<double>::reset(); }
    };

    /**
     * Facade for std::discrete_distribution
     */
    class DiscreteDistribution: public Distribution, private std::discrete_distribution<int>{
    public:
        explicit DiscreteDistribution(const std::vector<int>& weights):
            Distribution(), std::discrete_distribution<int>(weights.cbegin(), weights.cend()) {};

        double operator()() override {
            int x = std::discrete_distribution<int>::operator()(engine);
            return (double)x;
        }

        void reset() override { std::discrete_distribution<int>::reset(); }
    };

    /**
     * Facade for std::exponential_distribution
     * generates the interspike interval
     */
    class ExponentialDistribution: public Distribution, private std::exponential_distribution<double>{
    public:
        /**
         *
         * @param lambda mean firing rate
         */
        explicit ExponentialDistribution(double lambda): Distribution(), std::exponential_distribution<double>(lambda) {};

        double operator()() override {
            return std::exponential_distribution<double>::operator()(engine);
        }

        void reset() override { std::exponential_distribution<double>::reset(); }
    };


    /**
     * Facade for std::fisher_f_distribution
     * Generates a random number that is ratio of sum of m normally distributed values to the sum of n normally
     * distributed values
     */
    class FisherDistribution: public Distribution, private std::fisher_f_distribution<double>{
    public:
        explicit FisherDistribution(double m, double n): Distribution(), std::fisher_f_distribution<double>(m, n) {};

        double operator()() override {
            return std::fisher_f_distribution<double>::operator()(engine);
        }

        void reset() override { std::fisher_f_distribution<double>::reset(); }
    };

    /**
     * Facade for std::gamma_distribution
     * Sum of alpha exponentially distributed values with parameter beta
     */
    class GammaDistribution: public Distribution, private std::gamma_distribution<double>{
    public:
        explicit GammaDistribution(double alpha, double beta): Distribution(), std::gamma_distribution<double>(alpha, beta) {};

        double operator()() override{
            return std::gamma_distribution<double>::operator()(engine);
        }

        void reset() override { std::gamma_distribution<double>::reset(); };
    };

    /**
     * Facade for std::geometric_distribution
     * Generates a diration of interspike interval, int eh number of timestamps
     */
    class GeometricDistribution: public Distribution, private std::geometric_distribution<int>{
    public:
        /**
         *
         * @param p ratio of spike rate to the sample rate
         */
        explicit GeometricDistribution(double p): Distribution(), std::geometric_distribution<int>(p) {};

        double operator()() override {
            int number = std::geometric_distribution<int>::operator()(engine);
            return (double)number;
        }

        void reset() override { std::geometric_distribution<int>::reset(); }
    };

    /**
     * Facade for std::lognormal_distribution
     * Generates a number which logarithm is normally distributed
     */
    class LognormalDistribution: public Distribution, private std::lognormal_distribution<double>{
    public:
        explicit LognormalDistribution(double mean = 0.0, double std = 1.0): Distribution(),
            std::lognormal_distribution<double>(mean, std) {};

        double operator()() override {
            return std::lognormal_distribution<double>::operator()(engine);
        }

        void reset() override { std::lognormal_distribution<double>::reset(); }
    };

    /**
     * Facade for std::normal_distribution
     * Generates continuous random number with given mean and std distributed normally
     */
    class NormalDistribution: public Distribution, private std::normal_distribution<double>{
    public:
        explicit NormalDistribution(double mean = 0.0, double std = 1.0): Distribution(),
            std::normal_distribution<double>(mean, std) {};

        double operator()() override {
            return std::normal_distribution<double>::operator()(engine);
        }

        void reset() override { std::normal_distribution<double>::reset(); }
    };

    /**
     * Facade for std::piecewise_constant_distribution
     */
    class PiecewiseConstantDistribution: public Distribution, private std::piecewise_constant_distribution<double>{
    public:
        /**
         * Please mind that for any i ranging from 0 to intervals.size()-2 probability that the
         * value ranges from intervals[i] to intervals[i+1], is probabilities[i]/accumulate(probabilities.begin(),
         * probabilities.end(), 0)
         *
         * @param intervals
         * @param probabilities
         */
        PiecewiseConstantDistribution(const std::vector<double>& intervals, const std::vector<double>& probabilities):
            Distribution(), std::piecewise_constant_distribution<double>(intervals.cbegin(), intervals.cend(),
                    probabilities.cbegin()) {};

        double operator()() override {
            return std::piecewise_constant_distribution<double>::operator()(engine);
        }

        void reset() override { std::piecewise_constant_distribution<double>::reset(); }
    };

    /**
     * Facade for std::piecewise_linear_distribution
     */
    class PiecewiseLinearDistribution: public Distribution, private std::piecewise_linear_distribution<double>{
    public:
        /**
         *
         * @param intervals key points
         * @param values double numbers that are proportional to the p.d.f. at key points
         */
        PiecewiseLinearDistribution(const std::vector<double>& intervals, const std::vector<double>& values):
            Distribution(), std::piecewise_linear_distribution<double>(intervals.cbegin(), intervals.cend(),
                    values.cbegin()) {};

        double operator()() override {
            return std::piecewise_linear_distribution<double>::operator()(engine);
        }

        void reset() override { std::piecewise_linear_distribution<double>::reset(); }
    };

    /**
     * Facade for std::poission_distribution
     * Generates number of spikes within a certain interval
     */
    class PoissonDistribution: public Distribution, private std::poisson_distribution<int>{
    public:
        /**
         *
         * @param mean firing rate multiplied by the interval length
         */
        explicit PoissonDistribution(double mean = 1.0): Distribution(), std::poisson_distribution<int>(mean) {};

        double operator()() override {
            int N = std::poisson_distribution<int>::operator()(engine);
            return (double)N;
        }

        void reset() { std::poisson_distribution<int>::reset(); }
    };

    /**
     * Facede for std::student_t_distribution
     * Generates a number that is mean of n+1 normally distributed values with zero mean and 1.0 std
     */
    class StudentDistribution: public Distribution, private std::student_t_distribution<double>{
    public:
        explicit StudentDistribution(int n): Distribution(), std::student_t_distribution<double>(n) {};

        double operator()() override {
            return std::student_t_distribution<double>::operator()(engine);
        }

        void reset() { std::student_t_distribution<double>::reset(); }
    };

    /**
     * Facade for uniform int distribution
     * The distribution will select you an appropriate cell from the list and will place it to the given position
     */
    class UniformIntDistribution: public Distribution, private std::uniform_int_distribution<int>{
    public:

        explicit UniformIntDistribution(int min, int max): Distribution(),
            std::uniform_int_distribution<int>(min, max)
        {};

        double operator()() override {
            auto n = std::uniform_int_distribution<int>::operator()(engine);
            return (double)n;
        }

        void reset() override { std::uniform_int_distribution<int>::reset(); }
    };

    class UniformRealDistribution: public Distribution, private std::uniform_real_distribution<double>{
    public:
        explicit UniformRealDistribution(double min, double max): Distribution(),
            std::uniform_real_distribution<double>(min, max)  {};

        double operator()() override {
            return std::uniform_real_distribution<double>::operator()(engine);
        }

        void reset() override { std::uniform_real_distribution<double>::reset(); }
    };
}

#endif //MPI2_DISTRIBUTIONS_H
