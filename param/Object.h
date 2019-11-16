//
// Created by serik1987 on 28.08.19.
//

#ifndef MPI2_OBJECT_H
#define MPI2_OBJECT_H

#include <v8.h>
#include <exception>
#include <iostream>

namespace param {

    /**
     * Represents a parameter group or array of parameter groups
     *
     * This is not necessary to use V8 concepts (such as isolate, scope, context etc.) in order to use it.
     * Just follow the following rules:
     *
     * 1) Create a single instance of the parameter machine:
     * param::Engine(&argc, &argv) engine;
     *
     * 2) Get the root parameter:
     * param::Object& root = engine.getRoot()
     *
     * 3) Use the other methods mentioned below (except constructor, of course, it requires giving some V8 objects.
     *
     * Please, be carefull. If the instance of engine will be destroyed, all instances of this object will be damaged
     *
     * According to JS specification JS arrays are objects which properties have no string names. However, they
     * have indices (use getObjectField(idx) for getting an object field) and their number can be counted by means of
     * getPropertyNumber() routine
     */
    class Object {
    private:
        v8::Persistent<v8::Object> source;
        v8::Isolate* isolate;
        std::string object_name;

    public:

        /**
         * Constructor (for development use only
         *
         * @param isolate Created isolate
         * @param obj_name Object name that will be shown in the log and in error messages. An empty string can be passed
         * @param source An instance of Local<Object>. An instance of Persistent<Object> will be created during the
         * construction
         */
        Object(v8::Isolate* isolate, const std::string& obj_name, v8::Local<v8::Object> &source);
        Object(const Object&) = delete;

        /**
         * Releases the persistent instance of the JS object
         */
        ~Object();

        /**
         * Type of the field. FloatType corresponds to Number that is not Integer
         */
        enum FieldType {IntegerType, FloatType, StringType, ObjectType, BooleanType, NullType, UndefinedType};

        /**
         *
         * @param name Name of the field
         * @return value of the field if the value if integer
         * @throws TypeError if the field is not integer
         */
        [[nodiscard]] int getIntegerField(const std::string& name) const;

        /**
         *
         * @param name Name of the field
         * @return value of the field if the value is float
         * @throws TypeError if the field is not float
         */
        [[nodiscard]] double getFloatField(const std::string& name) const;

        /**
         * Returns a float item of the JS array
         *
         * @param index item index
         * @return copy of the item value
         * @throws TypeError if a certain item of the array is not float
         */
        [[nodiscard]] double getFloatField(uint32_t index) const;

        /**
         *
         * @param name Name of the field
         * @return value of the field as std::string instance if the field has String type
         * @throws TypeError if the field doesn't belong to the String type
         */
        [[nodiscard]] std::string getStringField(const std::string& name) const;

        /**
         *
         * @param name Name of the field
         * @return An instance of param::Object corresponding to this field, if the fieid value is Object
         * @throws TypeError if the field type is not Object
         */
        [[nodiscard]] param::Object getObjectField(const std::string& name) const;

        /**
         * Returns a certain item of an array, if it doesn't belong to an elementary type
         *
         * @param index item number, from 0 to N-1 where N is total number of items within the array
         * @return content for a certain property
         */
        [[nodiscard]] param::Object getObjectField(uint32_t index) const;

        /**
         *
         * @param name Name of the field
         * @return value of the field if it has a boolean type
         * @throws TypeError if the field doesn't belong to the Boolean type
         */
        [[nodiscard]] bool getBooleanField(const std::string& name) const;

        /**
         *
         * @param name Name of the field
         * @return Type of the field
         */
        [[nodiscard]] FieldType getFieldType(const std::string& name) const;

        /**
         *
         * @return JSON representation of the root object (named 'world')
         */
        [[nodiscard]] std::string stringify() const;

        /**
         * If an object represents an array, this returns total number of items within the array
         *
         * @return total number of all properties within the object
         */
        [[nodiscard]] uint32_t getPropertyNumber() const;


        /**
         * Provides iteration over all fields. Use it in the following way:
         *
         * for (std::string& field_name: param_object){
         *      .... // your code
         * }
         */
        class Iterator {
        private:
            const param::Object* parent;
            std::vector<std::string> propertyNames;
            int index;
        public:
            Iterator(const param::Object& parent, int index);

            const std::string& operator*(){
                if (index == propertyNames.size()){
                    throw std::range_error("in param::Object::iterator the end iterator is not accessible");
                }
                return propertyNames[index];
            }

            bool operator==(const Iterator& other){
                return (parent == other.parent && index == other.index);
            }

            bool operator!=(const Iterator& other){
                return !(*this == other);
            }

            bool operator>(const Iterator& other){
                return (parent == other.parent && index > other.index);
            }

            bool operator>=(const Iterator& other){
                return (parent == other.parent && index >= other.index);
            }

            bool operator<=(const Iterator& other){
                return (parent == other.parent && index <= other.index);
            }

            bool operator<(const Iterator& other){
                return (parent == other.parent && index < other.index);
            }

            const std::string& operator[](int n){
                if (n < 0 || n >= propertyNames.size()){
                    throw std::range_error("param::Object::iterator is out of range");
                }
                index = n;
                return propertyNames[index];
            }

            Iterator& operator++(){
                if (index == propertyNames.size()){
                    throw std::range_error("param::Object::iterator is out of range");
                }
                ++index;
                return *this;
            }

            Iterator operator++(int){
                if (index == propertyNames.size()){
                    throw std::range_error("param::Object::iterator is out of range");
                }
                Iterator other = *this;
                ++index;
                return other;
            }

            Iterator& operator--(){
                if (index == 0){
                    throw std::range_error("param::Object::iterator is out of range");
                }
                --index;
                return *this;
            }

            Iterator operator--(int){
                if (index == 0){
                    throw std::range_error("param::Object::iterator is out of range");
                }
                Iterator other = *this;
                --index;
                return other;
            }

            Iterator operator+(int n){
                Iterator other(*this);
                other.index = index + n;
                if (other.index > propertyNames.size()){
                    throw std::range_error("param::Object::iterator is out of range");
                }
                return other;
            }

            Iterator& operator+=(int n){
                index += n;
                if (index > propertyNames.size()){
                    throw std::range_error("param::Object::iterator is out of range");
                }
                return *this;
            }

            Iterator operator-(int n){
                Iterator other(*this);
                other.index = index - n;
                if (other.index < 0){
                    throw std::range_error("param::Object::iterator is out of range");
                }
                return other;
            }

            Iterator& operator-=(int n){
                index -= n;
                if (index < 0){
                    throw std::range_error("param::Object::iterator is out of range");
                }
                return *this;
            }

            int operator-(const Iterator& other){
                return index - other.index;
            }

        };

        /**
         *
         * @return Iterator pointed to the beginning of all fields
         */
        Iterator begin() const {
            return Iterator(*this, 0);
        }

        /**
         *
         * @return Iterator pointed to the end of the fields
         */
        Iterator end() const {
            return Iterator(*this, -1);
        }

    };
}

/**
 * Provides the output of the object
 *
 * @param out output stream
 * @param data the object itself
 * @return the output stream
 */
std::ostream& operator<<(std::ostream& out, const param::Object& data);


#endif //MPI2_OBJECT_H
