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
        virtual void loadParameterList(const Object& source)=0;
        virtual void broadcastParameterList()=0;
        virtual const char* getObjectType() const noexcept=0;

    public:
        void loadParameters(const Object& source){
            if (source.getStringField("type") != getObjectType()){
                throw WrongObjectType();
            }
            loadParameterList(source);
        }
    };
}

#endif //MPI2_LOADABLE_H
