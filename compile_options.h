/************************************************************************************************/
/*                                                                                              */
/* (C) Sergei Kozhukhov, a scientist in the Physiology of Sensory Systems Lab.,                 */
/*     the Institute of Higher Nervous Activity, Russian Academy of Sciences                    */
/* (C) the Institute of Higher Nervous Activity, Russian Academy of Sciences                    */
/* Correspondence address: 5A Butlerova str., 117485 Moscow, Russia                             */
/* E-mail: serik1987@gmail.com                                                                  */
/* Phone/What's App: +7 916 492 11 21                                                           */
/*                                                                                              */
/************************************************************************************************/

#ifndef MPI2_COMPILE_OPTIONS_H
#define MPI2_COMPILE_OPTIONS_H

/**
 * If this variable equals to 1 the application will fail any simulation job
 */
#define TEST_MODE 0

/**
 * Folder where the default model and standard libraries are located
 */
#define APP_FOLDER "/usr/local/lib/vis-brain/"

/**
 * Name of the application executable
 */
#define APP_NAME "vis-brain"

/**
 * Folder that contains default user model
 */
#define USER_FOLDER "~/.vis-brain/"

/**
 * File containing the default model and the default user model
 */
#define STANDARD_JS_OPTIONS "lib.js"

/**
 * A file where the project model will be defined
 */
#define STANDARD_PROJECT_OPTIONS "project.js"

/**
 * Alternative for --model key
 * --model my_model.js will use parameters written in the my_model.js file
 */
#define USER_DEFINED_MODEL_FILE_PARAMETER "model"

/**
 * Alternative for --set key. Parameters contained in Javascript code are expected to follow this key
 */
#define USER_DEFINED_MODEL_CODE_PARAMETER "set"

/**
 * Alternative for --update key. After this key model parameters in JSON format are expected to follow this key
 */
#define USER_DEFINED_MODEL_JSON_PARAMETER "update"

/**
 * When 1, the application will be run in debug mode. When 0, ordinary mode will be applied
 */
#define DEBUG 1

/******************************************************************************************************************/

/**
 * File that contains debugging information. Useless when DEBUG is 0.
 */
#define PUBLIC_DEBUG_LOG_FILE "debug.log"

/**
 * File that contains debugging information. Useless when DEBUG is 0. "/" means simulation output folder
 */
#define PRIVATE_DEBUG_LOG_FILE "/debug.log"

/**
 * File that contains all applied parameters. "/" means simulation output folder. Please, aolso bear in mind syslogd
 */
#define MODEL_DESCRIPTION_LOG_FILE "/model-description.log"

/**
 * File that stores the current progress status. "/" means simulation output folder. Please, also bear in mind syslogd
 */
#define NOTICE_LOG_FILE "/simulation.log"

/**
 * File that collects all warnings. "/" means simulation output folder. Please, also bear in mind Linux logging system
 */
#define WARNING_LOG_FILE "/warnings.log"

/**
 * Presence of this file indicates that all simulation jobs are failed. "/" means simulation output folder
 * Please, also bear in mind about Linux logging system
 */
#define FAIL_LOG_FILE "/failed.log"

/********************************************************************************************************************/

/**
 * The primary generator. The primary generator generates random seeds for the secondary generator.
 * Any functor is possible for such a function,
 * not only STL classes
 */
#define PRIMARY_GENERATOR std::random_device

/**
 * The secondary generator. The secondary generator immediately generates random number that will
 * be passed into certain C++ distribution through subclasses of data::noise::Distribution
 */
#define SECONDARY_GENERATOR std::mt19937

/**
 * When set to 1, the noise engine will be initialized immediately after loading world.application parameters
 * When set to 0, the noise engine will be initialized upon request
 */
#define AUTO_INITIALIZATION 1

#endif //MPI2_COMPILE_OPTIONS_H
