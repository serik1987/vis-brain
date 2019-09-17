//
// Created by serik1987 on 28.08.19.
//

#include "Object.h"
#include "Engine.h"
#include "exceptions.h"
#include <iostream>

using namespace v8;
using namespace param;

static int level = 0;

param::Object::Object(Isolate* iso, const std::string& obj_name, Local<v8::Object>& obj):
    isolate(iso), source(iso, obj), object_name(obj_name){
}

param::Object::~Object(){
    HandleScope handle_scope(isolate);
    source.Reset();
}

int param::Object::getIntegerField(const std::string &name) const {
    HandleScope handle_scope(isolate);
    Persistent<Context>& persistent_context = Engine::getInstance().getContext();
    Local<Context> context = Local<Context>::New(isolate, persistent_context);
    Local<v8::Object> local_source = Local<v8::Object>::New(isolate, source);
    MaybeLocal<Value> raw_prevalue = local_source->Get(context,
            String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked());
    Local<Value> raw_value = raw_prevalue.ToLocalChecked();
    if (!raw_value->IsInt32()){
        throw IntegerTypeError(object_name, name);
    }
    int value = raw_value->Int32Value(context).ToChecked();
    return value;
}

double param::Object::getFloatField(const std::string &name) const {
    HandleScope handle_scope(isolate);
    Persistent<Context>& persistent_context = Engine::getInstance().getContext();
    Local<Context> context = Local<Context>::New(isolate, persistent_context);
    Local<v8::Object> local_source = Local<v8::Object>::New(isolate, source);
    auto raw_value = local_source->Get(context,
            String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked()).ToLocalChecked();
    if (!raw_value->IsNumber()){
        throw FloatTypeError(object_name, name);
    }
    return raw_value->NumberValue(context).ToChecked();
}

std::string param::Object::getStringField(const std::string &name) const {
    HandleScope handle_scope(isolate);
    Persistent<Context>& persistent_context = Engine::getInstance().getContext();
    Local<Context> context = Local<Context>::New(isolate, persistent_context);
    Local<v8::Object> local_source = Local<v8::Object>::New(isolate, source);
    auto raw_value = local_source->Get(context,
            String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked()).ToLocalChecked();
    if (!raw_value->IsString()){
        throw StringTypeError(object_name, name);
    }
    Local<String> value = raw_value->ToString(context).ToLocalChecked();
    String::Utf8Value value_rep(isolate, value);
    return std::string(*value_rep);
}

param::Object param::Object::getObjectField(const std::string &name) const {
    HandleScope handle_scope(isolate);
    Persistent<Context>& persistent_context = Engine::getInstance().getContext();
    Local<Context> context = Local<Context>::New(isolate, persistent_context);
    // Local<Context> context = isolate->GetCurrentContext();
    Local<v8::Object> local_source = Local<v8::Object>::New(isolate, source);
    auto raw_value = local_source->Get(context,
            String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked()).ToLocalChecked();
    if (!raw_value->IsObject()){
        throw ObjectTypeError(object_name, name);
    }
    Local<v8::Object> value = raw_value->ToObject(context).ToLocalChecked();

    return param::Object(isolate, name, value);
}

bool param::Object::getBooleanField(const std::string &name) const {
    HandleScope handle_scope(isolate);
    Persistent<Context>& persistent_context = Engine::getInstance().getContext();
    Local<Context> context = Local<Context>::New(isolate, persistent_context);
    Local<v8::Object> local_source = Local<v8::Object>::New(isolate, source);
    auto raw_value = local_source->Get(context,
            String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked()).ToLocalChecked();
    if (!raw_value->IsBoolean()){
        throw BooleanTypeError(object_name, name);
    }
    return raw_value->BooleanValue(isolate);
}

param::Object::FieldType param::Object::getFieldType(const std::string &name) const {
    HandleScope handle_scope(isolate);
    Persistent<Context>& persistent_context = Engine::getInstance().getContext();
    Local<Context> context = Local<Context>::New(isolate, persistent_context);
    // Local<Context> context = isolate->GetCurrentContext();
    Local<v8::Object> local_source = Local<v8::Object>::New(isolate, source);
    Local<Value> data = local_source->Get(context, String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked())
            .ToLocalChecked();
    if (data->IsNull()){
        return NullType;
    } if (data->IsObject()){
        return ObjectType;
    } else if (data->IsString()) {
        return StringType;
    } else if (data->IsBoolean()){
        return BooleanType;
    } else if (data->IsInt32()){
        return IntegerType;
    } else if (data->IsNumber()){
        return FloatType;
    } else if (data->IsUndefined()){
        return UndefinedType;
    }
    return IntegerType;
}

param::Object::Iterator::Iterator(const param::Object &par, int ind): parent(&par), propertyNames(),
        index(ind) {
    HandleScope handle_scope(parent->isolate);
    Persistent<Context>& persistent_context = Engine::getInstance().getContext();
    Local<Context> context = Local<Context>::New(parent->isolate, persistent_context);
    Local<v8::Object> local_source = Local<v8::Object>::New(parent->isolate, parent->source);
    Local<Array> property_names = local_source->GetPropertyNames(context).ToLocalChecked();
    for (int i=0; i < property_names->Length(); i++){
        Local<Value> property_name_raw = property_names->Get(context, i).ToLocalChecked();
        if (property_name_raw->IsString()) {
            String::Utf8Value property_name(parent->isolate, property_name_raw);
            propertyNames.push_back(*property_name);
        } else {
            propertyNames.push_back(std::to_string(i));
        }
    }
    if (index == -1) index = propertyNames.size();
}

std::ostream& operator<<(std::ostream& out, const param::Object& obj){
    if (level==0){
        out << "Please, check the following simulation parameters\n";
    }
    bool paramExist = false;
    for (auto name: obj){
        paramExist = true;
        for (int i=0; i < level; i++){
            out << "\t";
        }
        out << name << ": ";
        switch (obj.getFieldType(name)){
            case param::Object::ObjectType:
                out << "\033[34mparameter group with the following details\033[0m\n";
                level++;
                out << obj.getObjectField(name);
                level--;
                break;
            case param::Object::StringType:
                out << obj.getStringField(name);
                break;
            case param::Object::FloatType:
                out << obj.getFloatField(name);
                break;
            case param::Object::BooleanType:
                if (obj.getBooleanField(name)){
                    out << "YES";
                } else {
                    out << "NO";
                }
                break;
            case param::Object::NullType:
                out << "(null)";
                break;
            case param::Object::UndefinedType:
                out << "(undefined)";
                break;
            case param::Object::IntegerType:
                out << obj.getIntegerField(name);
                break;
            default:
                out << "\033[31mUnknown or unsupported object field\033[0m";
        }
        if (obj.getFieldType(name) != param::Object::ObjectType) {
            out << std::endl;
        }
    }
    if (!paramExist){
        for (int i=0; i<level; i++){
            out << "\t";
        }
        std::cout << "No parameters in the group\n";
    }
    return out;
}


std::string param::Object::stringify() const{
    HandleScope handle_scope(isolate);
    Persistent<Context>& persistent_context = Engine::getInstance().getContext();
    Local<Context> context = Local<Context>::New(isolate, persistent_context);
    Context::Scope context_scope(context);
    TryCatch try_catch(isolate);
    std::string raw_code = "JSON.stringify(world)";
    Local<String> code = String::NewFromUtf8(isolate, raw_code.c_str()).ToLocalChecked();
    Local<Script> script = Script::Compile(context, code).ToLocalChecked();
    Local<Value> result = script->Run(context).ToLocalChecked();
    String::Utf8Value value(isolate, result);
    return *value;
}
