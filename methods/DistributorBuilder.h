//
// Created by serik1987 on 30.11.2019.
//

#ifndef MPI2_DISTRIBUTORBUILDER_H
#define MPI2_DISTRIBUTORBUILDER_H

#include "Distributor.h"
#include "../param/Loadable.h"

namespace method {

    /**
     * Auxiliary object that loads parameters from the JS file and creates
     * the corresponding distributor
     */
    class DistributorBuilder: public param::Loadable {
    private:
        mpi::Communicator& comm;
        Method& method;
        std::string distributor_name;

    protected:
        [[nodiscard]] const char* getObjectType() const noexcept override { return "application"; }
        void loadParameterList(const param::Object& source) override;
        void broadcastParameterList() override;
        void setParameter(const std::string& name, const void* pvalue) override {
            throw param::IncorrectParameterName(name, "distributor builder");
        }

    public:
        DistributorBuilder(mpi::Communicator& c, Method& m): comm(c), method(m) {};

        /**
         * Creates the distributor when all its parameters were loaded and broadcasted
         * Doesn't destroy it. This is your responsibility to destroy the object
         *
         * @return pointer to the distributor
         */
        Distributor* build();

        class incorrect_distributor: public std::exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "The distributor which name is given is incorrect or unsupported";
            }
        };
    };

}


#endif //MPI2_DISTRIBUTORBUILDER_H
