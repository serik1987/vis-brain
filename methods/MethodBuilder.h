//
// Created by serik1987 on 30.11.2019.
//

#ifndef MPI2_METHODBUILDER_H
#define MPI2_METHODBUILDER_H

#include "../param/Loadable.h"
#include "Method.h"

namespace method {

    /**
     * This is an auxiliary class that is very helpful to construct an arbitrary integration
     * method based on a certain JS class
     */
    class MethodBuilder: public param::Loadable {
    private:
        double integration_step = -1.0;
        std::string integration_method;

    protected:
        [[nodiscard]] const char* getObjectType() const noexcept override { return "application"; }
        void loadParameterList(const param::Object& source) override;
        void broadcastParameterList() override;
        void setParameter(const std::string& name, const void* pvalue) override {
            throw param::IncorrectParameterName(name, "method builder");
        }

    public:
        MethodBuilder() = default;

        /**
         *
         * @return integration step in ms
         */
        [[nodiscard]] double getIntegrationStep() const { return integration_step; }

        /**
         *
         * @return integration method to build
         */
        [[nodiscard]] const std::string& getIntegrationMethod() const { return integration_method; }

        class incorrect_integration_step: public std::exception{
        public:
            [[nodiscard]] const char* what() const noexcept override {
                return "Integration step is negative or zero";
            }
        };

        class incorrect_method: public std::exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "The integration method is incorrect of unsupported";
            }
        };

        /**
         * Sets the integration step
         *
         * @param value integration step in ms
         */
        void setIntegrationStep(double value) {
            if (value > 0){
                integration_step = value;
            } else {
                throw incorrect_integration_step();
            }
        }

        /**
         * Sets the integration method
         *
         * @param method method name
         */
        void setIntegrationMethod(const std::string& method) { integration_method = method; }

        Method* build();
    };

}


#endif //MPI2_METHODBUILDER_H
