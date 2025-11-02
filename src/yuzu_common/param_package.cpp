// SPDX-FileCopyrightText: 2017 Citra Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <array>
#include <stdexcept>
#include <utility>
#include <vector>

#include "yuzu_common/logging/log.h"
#include "yuzu_common/param_package.h"
#include "yuzu_common/string_util.h"

namespace Common {

constexpr char KEY_VALUE_SEPARATOR = ':';
constexpr char PARAM_SEPARATOR = ',';

constexpr char ESCAPE_CHARACTER = '$';
constexpr char KEY_VALUE_SEPARATOR_ESCAPE[] = "$0";
constexpr char PARAM_SEPARATOR_ESCAPE[] = "$1";
constexpr char ESCAPE_CHARACTER_ESCAPE[] = "$2";

/// A placeholder for empty param packages to avoid empty strings
/// (they may be recognized as "not set" by some frontend libraries like qt)
constexpr char EMPTY_PLACEHOLDER[] = "[empty]";

ParamPackage::ParamPackage(const std::string& serialized) {
    if (serialized == EMPTY_PLACEHOLDER) {
        return;
    }

    std::vector<std::string> pairs;
    Common::SplitString(serialized, PARAM_SEPARATOR, pairs);

    for (const std::string& pair : pairs) {
        std::vector<std::string> key_value;
        Common::SplitString(pair, KEY_VALUE_SEPARATOR, key_value);
        if (key_value.size() != 2) {
            LOG_ERROR(Common, "invalid key pair {}", pair);
            continue;
        }

        for (std::string& part : key_value) {
            part = Common::ReplaceAll(part, KEY_VALUE_SEPARATOR_ESCAPE, {KEY_VALUE_SEPARATOR});
            part = Common::ReplaceAll(part, PARAM_SEPARATOR_ESCAPE, {PARAM_SEPARATOR});
            part = Common::ReplaceAll(part, ESCAPE_CHARACTER_ESCAPE, {ESCAPE_CHARACTER});
        }

        Set(key_value[0], std::move(key_value[1]));
    }
}

ParamPackage::ParamPackage(const IParamPackage& param) :
    ParamPackage(std::string(param.Serialize()))
{
}

ParamPackage::ParamPackage(std::initializer_list<DataType::value_type> list) : data(list) {}

const char * ParamPackage::Serialize() const {
    if (data.empty())
        return EMPTY_PLACEHOLDER;

    std::string result;

    for (const auto& pair : data) {
        std::array<std::string, 2> key_value{{pair.first, pair.second}};
        for (std::string& part : key_value) {
            part = Common::ReplaceAll(part, {ESCAPE_CHARACTER}, ESCAPE_CHARACTER_ESCAPE);
            part = Common::ReplaceAll(part, {PARAM_SEPARATOR}, PARAM_SEPARATOR_ESCAPE);
            part = Common::ReplaceAll(part, {KEY_VALUE_SEPARATOR}, KEY_VALUE_SEPARATOR_ESCAPE);
        }
        result += key_value[0] + KEY_VALUE_SEPARATOR + key_value[1] + PARAM_SEPARATOR;
    }

    result.pop_back(); // discard the trailing PARAM_SEPARATOR
    m_serializeData = result;
    return m_serializeData.c_str();
}

const  std::string * ParamPackage::GetRaw(const std::string& key) const
{
    auto pair = data.find(key);
    if (pair == data.end()) 
    {
        return nullptr;
    }
    return &pair->second;
}

std::string ParamPackage::Get(const std::string& key, const std::string& default_value) const {
    auto pair = data.find(key);
    if (pair == data.end()) {
        LOG_TRACE(Common, "key '{}' not found", key);
        return default_value;
    }

    return pair->second;
}

int ParamPackage::Get(const std::string& key, int default_value) const {
    auto pair = data.find(key);
    if (pair == data.end()) {
        LOG_TRACE(Common, "key '{}' not found", key);
        return default_value;
    }

    try {
        return std::stoi(pair->second);
    } catch (const std::logic_error&) {
        LOG_ERROR(Common, "failed to convert {} to int", pair->second);
        return default_value;
    }
}

float ParamPackage::Get(const std::string& key, float default_value) const {
    auto pair = data.find(key);
    if (pair == data.end()) {
        LOG_TRACE(Common, "key {} not found", key);
        return default_value;
    }

    try {
        return std::stof(pair->second);
    } catch (const std::logic_error&) {
        LOG_ERROR(Common, "failed to convert {} to float", pair->second);
        return default_value;
    }
}

void ParamPackage::Set(const std::string& key, std::string value) {
    data.insert_or_assign(key, std::move(value));
}

void ParamPackage::Set(const std::string& key, int value) {
    data.insert_or_assign(key, std::to_string(value));
}

void ParamPackage::Set(const std::string& key, float value) {
    data.insert_or_assign(key, std::to_string(value));
}

bool ParamPackage::Has(const std::string& key) const {
    return data.find(key) != data.end();
}

void ParamPackage::Erase(const std::string& key) {
    data.erase(key);
}

void ParamPackage::Clear() {
    data.clear();
}

} // namespace Common

IParamPackageListImpl::IParamPackageListImpl(const std::vector<Common::ParamPackage> & list)
{
    m_list.reserve(list.size());
    for (const Common::ParamPackage & package : list) 
    {
        m_list.emplace_back(new IParamPackageImpl(package));
    }
}

IParamPackageListImpl::~IParamPackageListImpl()
{
    for (IParamPackageImpl * item : m_list)
    {
        item->Release();
    }
}

uint32_t IParamPackageListImpl::GetCount() const
{
    return (uint32_t)m_list.size();
}

IParamPackage & IParamPackageListImpl::GetParamPackage(uint32_t index) const
{
    return *m_list[index];
}

void IParamPackageListImpl::Release()
{
    delete this;
}

IParamPackageImpl::IParamPackageImpl(const Common::ParamPackage & package) :
    m_package(package)
{
}

bool IParamPackageImpl::Has(const char * key) const
{
    return m_package.Has(key);
}

bool IParamPackageImpl::GetBool(const char * key, bool default_value) const
{
    return m_package.Get(key, default_value);
}

int32_t IParamPackageImpl::GetInt(const char * key, int32_t default_value) const
{
    return m_package.Get(key, default_value);
}

float IParamPackageImpl::GetFloat(const char* key, float default_value) const
{
    return m_package.Get(key, default_value);
}

const char * IParamPackageImpl::GetString(const char * key, const char * default_value) const
{
    const std::string * ptr = m_package.GetRaw(std::string(key));
    if (ptr != nullptr) {
        return ptr->c_str();
    }
    return default_value;
}

const char * IParamPackageImpl::Serialize() const
{
    return m_package.Serialize();
}

void IParamPackageImpl::Release()
{
    delete this;
}
