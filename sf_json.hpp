#pragma once

#include "sf_lex.hpp"
#include "sf_yacc.hpp"
#include "sf_http_utils.h"
#include "sf_json.h"
#include "sf_utils.hpp"
#include "sf_define.h"


namespace skyfire
{
    inline sf_json sf_json::from_string(const std::string &json_str)
    {
        sf_lex lex;
        lex.add_rules({
                              {"string",               R"("([^\\"]|(\\["\\/bnrt]|(u[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F])))*")"},
                              {"left_square_bracket",  R"(\[)"},
                              {"right_square_bracket", R"(\])"},
                              {"left_curly_bracket",   R"(\{)"},
                              {"right_curly_bracket",  R"(\})"},
                              {"comma",                R"(,)"},
                              {"colon",                R"(:)"},
                              {"ws",                   R"([\r\n\t ]+)"},
                              {"number",               R"(-?(0|[1-9]\d*)(\.\d+)?(e|E(\+|-)?0|[1-9]\d*)?)"},
                              {"true",                 R"(true)"},
                              {"false",                R"(false)"},
                              {"null",                 R"(null)"}
                      });

        std::vector<sf_lex_result_t> lex_result;
        if (!lex.parse(json_str, lex_result))
        {
            return sf_json();
        }
        lex_result.erase(std::remove_if(lex_result.begin(), lex_result.end(), [](const sf_lex_result_t &r)
        {
            return r.id == "ws";
        }), lex_result.end());
        sf_yacc yacc;
        yacc.add_rules({
                               {
                                       "value",
                                       {
                                               {
                                                       {"object"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           return data[0]->user_data;
                                                       }
                                               },
                                               {
                                                       {"array"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           return data[0]->user_data;
                                                       }
                                               },
                                               {
                                                       {"string"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           return sf_json(sf_json::json_string_to_string(data[0]->text));
                                                       }
                                               },
                                               {
                                                       {"number"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           long double tmp_num;
                                                           sscanf(data[0]->text.c_str(), "%Lf", &tmp_num);
                                                           return sf_json(tmp_num);
                                                       }
                                               },
                                               {
                                                       {"true"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           return sf_json(true);
                                                       }
                                               },
                                               {
                                                       {"false"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           return sf_json(false);
                                                       }
                                               },
                                               {
                                                       {"null"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           return sf_json();
                                                       }
                                               }
                                       }
                               },
                               {
                                       "object",
                                       {
                                               {
                                                       {"left_curly_bracket",  "right_curly_bracket"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_object();
                                                           return json;
                                                       }
                                               },
                                               {
                                                       {"left_curly_bracket",  "members",  "right_curly_bracket"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           return data[1]->user_data;
                                                       }
                                               }
                                       }
                               },
                               {
                                       "members",
                                       {
                                               {
                                                       {"member"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           return data[0]->user_data;
                                                       }
                                               },
                                               {
                                                       {"members",             "comma",    "member"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_object();
                                                           sf_json js1 = std::any_cast<sf_json>(data[0]->user_data);
                                                           sf_json js2 = std::any_cast<sf_json>(data[2]->user_data);
                                                           json.join(js1);
                                                           json.join(js2);
                                                           return json;
                                                       }
                                               },
                                               {
                                                       {"members","comma","members"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_object();
                                                           sf_json js1 = std::any_cast<sf_json>(data[0]->user_data);
                                                           sf_json js2 = std::any_cast<sf_json>(data[2]->user_data);
                                                           json.join(js1);
                                                           json.join(js2);
                                                           return json;
                                                       }
                                               }
                                       }
                               },
                               {
                                       "member",
                                       {
                                               {
                                                       {"string",              "colon", "array"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_object();
                                                           auto json1 = std::any_cast<sf_json>(data[2]->user_data);

                                                           json[sf_json::json_string_to_string(
                                                                   data[0]->text)] = json1;
                                                           return json;
                                                       }
                                               },
                                               {
                                                       {"string",              "colon",    "object"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_object();
                                                           json[sf_json::json_string_to_string(
                                                                   data[0]->text)] = std::any_cast<sf_json>(
                                                                   data[2]->user_data);
                                                           return json;
                                                       }
                                               },
                                               {
                                                       {"string",   "colon", "string"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_object();
                                                           json[sf_json::json_string_to_string(data[0]->text)] = sf_json::json_string_to_string(
                                                                   data[2]->text);
                                                           return json;
                                                       }
                                               },
                                               {
                                                       {"string", "colon", "number"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_object();
                                                           long double tmp_num;
                                                           sscanf(data[2]->text.c_str(), "%Lf", &tmp_num);
                                                           json[sf_json::json_string_to_string(
                                                                   data[0]->text)] = tmp_num;
                                                           return json;
                                                       }
                                               },
                                               {
                                                       {"string", "colon", "true"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_object();
                                                           json[sf_json::json_string_to_string(data[0]->text)] = true;
                                                           return json;
                                                       }
                                               },
                                               {
                                                       {"string", "colon", "false"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_object();
                                                           json[sf_json::json_string_to_string(data[0]->text)] = false;
                                                           return json;
                                                       }
                                               },
                                               {
                                                       {"string", "colon", "null"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_object();
                                                           json[sf_json::json_string_to_string(
                                                                   data[0]->text)] = sf_json();
                                                           return json;
                                                       }
                                               }

                                       }
                               },
                               {
                                       "array",
                                       {
                                               {
                                                       {"left_square_bracket", "right_square_bracket"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_array();
                                                           return json;
                                                       }
                                               },
                                               {
                                                       {"left_square_bracket", "values", "right_square_bracket"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           return data[1]->user_data;
                                                       }
                                               }
                                       }
                               },
                               {
                                       "values",
                                       {
                                               {
                                                       {"value"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_array();
                                                           json.append(std::any_cast<sf_json>(data[0]->user_data));
                                                           return json;
                                                       }
                                               },
                                               {
                                                       {"values",            "comma",    "string"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_array();
                                                           sf_json json1 = std::any_cast<sf_json>(data[0]->user_data);
                                                           sf_json json2(sf_json::json_string_to_string(data[2]->text));
                                                           json.join(json1);
                                                           json.append(json2);
                                                           return json;
                                                       }
                                               },
                                               {
                                                       {"values",            "comma",    "number"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_array();
                                                           sf_json json1 = std::any_cast<sf_json>(data[0]->user_data);
                                                           long double tmp_num;
                                                           sscanf(data[2]->text.c_str(), "%Lf", &tmp_num);
                                                           sf_json json2(tmp_num);
                                                           json.join(json1);
                                                           json.append(json2);
                                                           return json;
                                                       }
                                               },
                                               {
                                                       {"values",            "comma",    "object"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_array();
                                                           sf_json json1 = std::any_cast<sf_json>(data[0]->user_data);
                                                           sf_json json2 = std::any_cast<sf_json>(data[2]->user_data);
                                                           json.join(json1);
                                                           json.append(json2);
                                                           return json;
                                                       }
                                               },
                                               {
                                                       {"values",            "comma",    "array"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_array();
                                                           sf_json json1 = std::any_cast<sf_json>(data[0]->user_data);
                                                           sf_json json2 = std::any_cast<sf_json>(data[2]->user_data);
                                                           json.join(json1);
                                                           json.append(json2);
                                                           return json;
                                                       }
                                               },
                                               {
                                                       {"values",            "comma",    "true"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_array();
                                                           sf_json json1 = std::any_cast<sf_json>(data[0]->user_data);
                                                           sf_json json2(true);
                                                           json.join(json1);
                                                           json.append(json2);
                                                           return json;
                                                       }
                                               },
                                               {
                                                       {"values",            "comma",    "false"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_array();
                                                           sf_json json1 = std::any_cast<sf_json>(data[0]->user_data);
                                                           sf_json json2(false);
                                                           json.join(json1);
                                                           json.append(json2);
                                                           return json;
                                                       }
                                               },
                                               {
                                                       {"values",            "comma",    "null"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_array();
                                                           sf_json json1 = std::any_cast<sf_json>(data[0]->user_data);
                                                           sf_json json2();
                                                           json.join(json1);
                                                           json.append(json2);
                                                           return json;
                                                       }
                                               },
                                               {
                                                       {"values","comma","values"},
                                                       [](const std::vector<std::shared_ptr<sf_yacc_result_t>> &data) -> std::any
                                                       {
                                                           sf_json json;
                                                           json.convert_to_array();
                                                           sf_json json1 = std::any_cast<sf_json>(data[0]->user_data);
                                                           sf_json json2 = std::any_cast<sf_json>(data[2]->user_data);
                                                           json.join(json1);
                                                           json.join(json2);
                                                           return json;
                                                       }
                                               }
                                       }
                               }
                       });

        yacc.add_termanate_ids({"value", "array", "object"});
        std::vector<std::shared_ptr<sf_yacc_result_t>> yacc_result;
        if (!yacc.parse(lex_result, yacc_result))
        {
            return sf_json();
        }
        return std::any_cast<sf_json>(yacc_result[0]->user_data);
    }

    inline sf_json::sf_json() : value__(std::make_shared<sf_json_value>())
    {
        value__->type = sf_json_type::null;
    }

    inline sf_json::sf_json(const std::string &str) : sf_json()
    {
        value__->type = sf_json_type::string;
        value__->string_value = str;
    }

    inline sf_json::sf_json(const char* c_str) : sf_json(std::string(c_str))
    {
    }



    inline sf_json::sf_json(long double number):sf_json()
    {
        value__->type = sf_json_type::number;
        value__->number_value = number;
    }

    inline sf_json::sf_json(const sf_json &json):sf_json()
    {
        value__=json.value__;
    }

    inline sf_json::sf_json(bool boolean_value) : sf_json()
    {
        value__->type = sf_json_type::boolean;
        value__->number_value = boolean_value;
    }


    inline sf_json::sf_json(const std::shared_ptr<sf_json_value> &value)
    {
        value__ = value;
    }

    inline void sf_json::convert_to_object()
    {
        if (value__->type == sf_json_type::object)
        {
            return;
        }
        value__->type = sf_json_type::object;
        clear();
    }

    inline void sf_json::convert_to_array()
    {
        if (value__->type == sf_json_type::array)
        {
            return;
        }
        value__->type = sf_json_type::array;
        clear();
    }

    inline sf_json_type sf_json::type() const
    {
        return value__->type;
    }


    inline const sf_json sf_json::operator[](const std::string &key) const
    {
        if (value__->type != sf_json_type::object)
        {
            return sf_json();
        }
        if (value__->object_value.count(key) == 0)
        {
            return sf_json();
        }
        return sf_json(value__->object_value[key]);
    }

    inline sf_json sf_json::operator[](const std::string &key)
    {
        if (value__->type != sf_json_type::object)
        {
            convert_to_object();
        }
        if (value__->object_value.count(key) == 0)
        {
            value__->object_value[key] = sf_json().value__;
        }
        return sf_json(value__->object_value[key]);
    }

    inline const sf_json sf_json::operator[](int key) const
    {
        if (value__->type != sf_json_type::array)
        {
            return sf_json();
        }
        if (value__->array_value.size() <= key)
        {
            return sf_json();
        }
        return sf_json(value__->array_value[key]);
    }

    inline sf_json::operator std::string() const
    {
        switch (value__->type)
        {
            case sf_json_type::array:
            case sf_json_type::object:
            case sf_json_type::null:
                return "";
            case sf_json_type::boolean:
                if (value__->number_value != 0)
                {
                    return "true";
                } else
                {
                    return "false";
                }
            case sf_json_type::number:
                return std::to_string(value__->number_value);
            case sf_json_type::string:
                return value__->string_value;
            default:
                return "";
        }
    }

    inline std::string sf_json::json_string_to_string(std::string json_str)
    {
        json_str.erase(json_str.begin());
        json_str.pop_back();
        sf_string_replace(json_str,"\\\"","\"");
        sf_string_replace(json_str,"\\/","/");
        sf_string_replace(json_str,"\\b","\b");
        sf_string_replace(json_str,"\\f","\f");
        sf_string_replace(json_str,"\\n","\n");
        sf_string_replace(json_str,"\\r","\r");
        sf_string_replace(json_str,"\\t","\t");

        // todo 未处理unicode

        sf_string_replace(json_str,"\\\\","\\");

        return json_str;
    }

    inline std::string sf_json::string_to_json_string(std::string str)
    {
        sf_string_replace(str,"\\","\\\\");
        sf_string_replace(str,"\"","\\\"");
        sf_string_replace(str,"/","\\/");
        sf_string_replace(str,"\b","\\b");
        sf_string_replace(str,"\f","\\f");
        sf_string_replace(str,"\n","\\n");
        sf_string_replace(str,"\r","\\r");
        sf_string_replace(str,"\t","\\t");
        // todo 未处理unicode
        str.insert(str.begin(), '\"');
        str += "\"";
        return str;
    }

    inline sf_json::operator bool() const
    {
        switch (value__->type)
        {
            case sf_json_type::array:
                return !value__->array_value.empty();
            case sf_json_type::object:
                return !value__->object_value.empty();
            case sf_json_type::null:
                return false;
            case sf_json_type::boolean:
            case sf_json_type::number:
                return value__->number_value != 0;
            case sf_json_type::string:
                return !value__->string_value.empty();
            default:
                return false;
        }
    }

    inline sf_json &sf_json::operator=(long double value)
    {
        if (value__->type != sf_json_type::number)
        {
            value__->type = sf_json_type::number;
            clear();
        }
        value__->number_value = value;
        return *this;
    }

    inline sf_json &sf_json::operator=(const std::string &value)
    {
        if (value__->type != sf_json_type::string)
        {
            value__->type = sf_json_type::string;
            clear();
        }
        value__->string_value = value;
        return *this;
    }

    inline void sf_json::append(const sf_json &value)
    {
        convert_to_array();
        value__->array_value.push_back(value.value__);
    }

    inline sf_json sf_json::clone() const
    {
        sf_json tmp_json;
        value_copy__(value__, tmp_json.value__);
        return tmp_json;
    }

    inline bool sf_json::join(const sf_json &other)
    {
        if (value__->type == sf_json_type::array && other.value__->type == sf_json_type::array)
        {
            value__->array_value.insert(value__->array_value.end(),
                                        other.value__->array_value.begin(),
                                        other.value__->array_value.end()
            );
            return true;
        } else if (value__->type == sf_json_type::object && other.value__->type == sf_json_type::object)
        {
            value__->object_value.insert(other.value__->object_value.begin(),
                                         other.value__->object_value.end());
            return true;
        }
        return false;
    }

    inline void sf_json::clear()
    {
        value__->number_value = 0;
        value__->string_value.clear();
        value__->array_value.clear();
        value__->object_value.clear();
    }

    inline void sf_json::resize(int size)
    {
        convert_to_array();
        value__->array_value.resize(size);
    }

    inline void sf_json::remove(int pos)
    {
        if (value__->array_value.size() > pos)
        {
            value__->array_value.erase(value__->array_value.begin() + pos);
        }
    }

    inline void sf_json::remove(int pos, int len)
    {
        if (value__->array_value.size() > pos)
        {
            if (len > value__->array_value.size() - pos)
            {
                len = static_cast<int>(value__->array_value.size() - pos);
            }
            value__->array_value.erase(value__->array_value.begin() + pos, value__->array_value.begin() + pos + len);
        }
    }

    inline void sf_json::remove(const std::string &key)
    {
        value__->object_value.erase(key);
    }

    inline std::string sf_json::to_string() const
    {
        std::string ret;
        switch (value__->type)
        {
            case sf_json_type::object:
            {
                ret += "{";
                for (auto &p:value__->object_value)
                {
                    ret += string_to_json_string(p.first) + ":" + sf_json(p.second).to_string() + ",";
                }
                if (ret.back() == ',')
                    ret.pop_back();
                ret += "}";
            }
                break;
            case sf_json_type::array:
            {
                ret += "[";
                for (auto &p:value__->array_value)
                {
                    ret += sf_json(p).to_string() + ",";
                }
                if (ret.back() == ',')
                    ret.pop_back();
                ret += "]";
            }
                break;
            case sf_json_type::string:
                ret += string_to_json_string(value__->string_value);
                break;
            case sf_json_type::number:
            {
                char buffer[SF_DEFAULT_BUFFER_SIZE];
                sprintf(buffer,"%Lg",value__->number_value);
                ret+=buffer;
                break;
            }
            case sf_json_type::boolean:
                ret += value__->number_value != 0 ? "true" : "false";
                break;
            case sf_json_type::null:
                ret += "null";
                break;
        }
        return ret;
    }


    inline sf_json::operator long double() const
    {
        switch (value__->type)
        {
            case sf_json_type::array:
            case sf_json_type::object:
            case sf_json_type::null:
            case sf_json_type::string:
                return 0;
            case sf_json_type::boolean:
                return value__->number_value != 0;
            case sf_json_type::number:
                return value__->number_value;
            default:
                return 0;
        }
    }

    inline sf_json &sf_json::operator=(const sf_json& value)
    {
        if(&value != this)
        {
            value__ = value.value__;
        }
        return *this;
    }

    inline void sf_json::value_copy__(const std::shared_ptr<sf_json_value>& src, std::shared_ptr<sf_json_value>& dst) const
    {
        dst->type = src->type;
        dst->string_value = src->string_value;
        dst->number_value = src->number_value;
        if(!src->array_value.empty())
        {
            for (int i=0;i<src->array_value.size();++i)
            {
                dst->array_value.push_back(std::make_shared<sf_json_value>());
                value_copy__(src->array_value[i], dst->array_value[i]);
            }
        }
        if(!src->object_value.empty())
        {
            for (auto &p:src->object_value)
            {
                dst->object_value[p.first] = std::make_shared<sf_json_value>();
                value_copy__(p.second, dst->object_value[p.first]);
            }
        }
    }

    inline const sf_json sf_json::operator[](const char *c_str) const
    {
        return operator[](std::string(c_str));
    }

    inline sf_json sf_json::operator[](const char *c_str)
    {
        return operator[](std::string(c_str));
    }

    inline sf_json sf_json::operator[](int key)
    {
        if (value__->type != sf_json_type::array)
        {
            convert_to_array();
        }
        if (value__->array_value.size() <= key)
        {
            resize(key+1);
        }
        return sf_json(value__->array_value[key]);
    }

    inline sf_json::operator long long() const
    {
        return static_cast<long long>(operator long double());
    }

    inline sf_json::sf_json(long long number):sf_json(static_cast<long double>(number))
    {
    }

    sf_json &sf_json::operator=(long long value)
    {
        if (value__->type != sf_json_type::number)
        {
            value__->type = sf_json_type::number;
            clear();
        }
        value__->number_value = value;
        return *this;
    }

    sf_json &sf_json::operator=(bool value)
    {
        if (value__->type != sf_json_type::boolean)
        {
            value__->type = sf_json_type::boolean;
            clear();
        }
        value__->number_value = value;
        return *this;
    }

    inline bool sf_json::is_null()
    {
        return value__->type == sf_json_type::null;
    }

    inline sf_json::sf_json(int number):sf_json(static_cast<long double>(number))
    {

    }

    inline sf_json::operator int() const
    {
        return static_cast<int>(operator long double());
    }

    inline sf_json &sf_json::operator=(int value)
    {
        return operator=(static_cast<long double>(value));
    }

    inline sf_json &sf_json::operator=(const char *value)
    {
        return operator=(std::string(value));
    }

    inline void sf_json::convert_to_null()
    {
        clear();
        value__->type = sf_json_type ::null;
    }

    inline size_t sf_json::size() const
    {
        return value__->type == sf_json_type ::array?value__->array_value.size():0;
    }

    inline bool sf_json::has(const std::string &key)
    {
        return value__->type == sf_json_type ::object?value__->object_value.count(key)!=0:0;
    }

    inline bool sf_json::has(const char *c_key)
    {
        return has(std::string(c_key));
    }

    inline sf_json operator ""_json(const char *str, unsigned long)
    {
        return sf_json::from_string(str);
    }

    inline std::ostream &operator<<(std::ostream &os, const sf_json &json)
    {
        return os<<json.to_string();
    }


}