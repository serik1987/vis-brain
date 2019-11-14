/**
 * In order to create new external stimuli, please:
 * 1) Fill in the functions given below by your own code
 * 2) Compile this file in the following way
 * gcc -fPIC -c my-stimulus.cpp
 * 3) Build this file as a shared object (.so) file:
 * gcc -shared -o stimuli/my-stimulus.so my-stimulus.o
 * Please bear in mind, that if you create a stimulus names "my-stimulus" its name shall be "my-stimulus.so"
 * and this shall be placed to a folder stimuli of the current directory
 * For a permanent use you may also place my-stimulus.so into /usr/loca/lib/vis-brain/stimuli folder (public access)
 * or ~/.vis-nrain/stimuli folder (your own private access
 *
 * Use the following JS code to apply your stimulus:
 * world.stimulus = {
 *      name: "processor",
 *      mechanism: "stimulus:external.my-stimulus",
 *      luminance: 0.5,
 *      contrast: 1.0,
 *      prestimulus_epoch: 400*ms,
 *      stimulus_duration: 400*ms,
 *      poststimulus_epoch: 400*ms,
 *      your_own_stimulus_parameter1: your_owm_value, // All other stimulus parameters can be only doubles
 *      ........                                      // Their full list shall be returned by the get_parameter_list
 *                                                    // function. Their values will be passed to this library by
 *                                                    // means of set_parameter_name function
 * }
 *
 * Also, we kindly ask you not to print anything to the standard output due to the following reasons:
 * 1) This may not be process-safe
 * 2) When vis-brain is launched from a certain client program, this may corrupt the data format and cause errors
 */

#include <cstring>
#include <cmath>

#define SIGMA_SPEED __sigma_speed

extern "C" {

#include <cstdio>

    static double __sigma_speed = 0.0;
    constexpr int __pnumber = 1;
    static const char* __parameters[] = {"sigma_speed"};

    /**
     * Returns list of all parameter names. Parameter types are implied to be double
     *
     * @param argc pointer to total number of all arguments
     * @param plist pointer to the array of strings that contains all parameter names vis-brain will look for
     * for JS object for revealing the parameter names. Parameter types are assumed to be double
     * @return 0 on success, any other value on failure
     */
    int get_parameter_list(int *argc, const char ***plist) {
        *argc = __pnumber;
        *plist = __parameters;
        return 0;
    }

    /**
     * Returns the value of a certain parameter
     *
     * @param name parameter name
     * @param pvalue pointer to the parameter value
     * @return 0 on success, any other value on failure
     */
    int get_parameter_value(const char* name, double* pvalue){
        int result;
        if (strcmp(name, "sigma_speed") == 0){
            *pvalue = __sigma_speed;
            result = 0;
        } else {
            result = -1;
        }
        return result;
    }

    /**
     * Sets the name of a certain parameter
     *
     * @param name parameter name
     * @param value parameter value
     * @return 0 on success, any other value on failure
     */
    int set_parameter_value(const char* name, double value){
        int result = -1;
        if (strcmp(name, "sigma_speed") == 0){
            __sigma_speed = value;
            result = 0;
        }
        return result;
    }

    /**
     * This function will be called before the start of the simulation but after all parameters were set
     *
     * @return 0 on success, any other value on failure; The failure will abort current simulation but will not
     * abort all other jobs
     */
    int initialize_buffers(){
        return 0;
    }

    /**
     * Returns the value of a certain pixels from the stimulus
     *
     * @param x pixel's abscissa in degrees. (0, 0) corresponds to the center of the visual field
     * @param y pixel's ordinate in degrees. (0, 0) corresponds to the center of the visual field
     * @param t time from the stimulus onset in ms (0.0 corresponds to the stimulus onset, not to the simulation
     * start)
     * @param pvalue pointer to the stimulus value. Shall be within [0.0 and 1.0] range. All values below 0.0 will be
     * automatically replaced by 0.0; all values above 1.0 will be automatically replaced by 1.0. After such transformation
     * another linear transformation will be performed in such a way as [0.0, 1.0] will be [L-0.5*C, L+0.5*C] interval
     * where L is a value of "luminance" property and C is a value of "contrast" property
     * @return 0 on success, any other value on failure
     */
    int get_stimulus_value(double x, double y, double t, double* pvalue){
        double sigma = 0.001 * SIGMA_SPEED * t;
        double sigma2 = sigma * sigma;
        if (t == 0){
            *pvalue = 0.0;
        } else {
            *pvalue = 2.0 * exp(-x * x / sigma2 - y * y / sigma2) + 0.25;
        }
        return 0;
    }

    /**
     * This function will be called after simulation will be completed. Please, bear in mind that the function
     * may be called twice, so, this is your responsibility to check whether it wants to deallocated
     * deallocated objects or doesn't deallocate anything. Please, note that
     * delete nullptr;
     * will deallocate nothing and is OK.
     * free(nullptr);
     * will definitely cause an error.
     * delete x;
     * will probably cause an error if this function will be called twice.
     * delete x;
     * x = nullptr;
     * is always OK. At first call this deallocate x, at second call it will do nothing.
     * However, it requires x to be allocated by means of the new operator
     */
    void finalize_buffers(){

    }

}