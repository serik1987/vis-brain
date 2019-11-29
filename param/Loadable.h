//
// Created by serik1987 on 28.08.19.
//

#ifndef MPI2_LOADABLE_H
#define MPI2_LOADABLE_H

#include "Object.h"
#include "exceptions.h"

namespace param{

    /**
     * Base class for all objects which parameters are loaded through the JS code
     */
    class Loadable{
    protected:
        /**
         * Loads the parameter list from the source
         * The load function first checks the value of the 'type' parameter. This value shall contain
         * the same string as returned by getObjectType() function. Failure to do this will throw
         * a WrongObjectType exception that is critical
         *
         * @param source set of parameters as param::Object object
         */
        virtual void loadParameterList(const Object& source)=0;

        /**
         * Moves all loaded parameters from the process with rank 0 to the other processes
         */
        virtual void broadcastParameterList()=0;

        /**
         * Definition of the object type is given in loadParameterList reference
         *
         * @return the object type
         */
        virtual const char* getObjectType() const noexcept=0;

    public:
        virtual ~Loadable() {};

        /**
         * Loads the parameters from the Object
         *
         * @param source the object that is a source of all parameters
         */
        void loadParameters(const Object& source);

        /**
         * Copies all parameters from the process with rank 0 to all other processes
         */
        void broadcastParameters();

        /**
         * Sets the parameter by name
         *
         * @param name parameter name
         * @param pvalue pointer to the parameter value
         */
        virtual void setParameter(const std::string& name, const void* pvalue)=0;
    };
}

#endif //MPI2_LOADABLE_H
