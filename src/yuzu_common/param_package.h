// SPDX-FileCopyrightText: 2017 Citra Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <initializer_list>
#include <string>
#include <unordered_map>
#include <nxemu-module-spec/operating_system.h>

namespace Common {

/// A string-based key-value container supporting serializing to and deserializing from a string
class ParamPackage {
public:
    using DataType = std::unordered_map<std::string, std::string>;

    ParamPackage() = default;
    explicit ParamPackage(const std::string& serialized);
    ParamPackage(const IParamPackage& param);
    ParamPackage(std::initializer_list<DataType::value_type> list);
    ParamPackage(const ParamPackage& other) = default;
    ParamPackage(ParamPackage&& other) noexcept = default;

    ParamPackage& operator=(const ParamPackage& other) = default;
    ParamPackage& operator=(ParamPackage&& other) = default;

    [[nodiscard]] const char * Serialize() const;
    [[nodiscard]] const std::string * GetRaw(const std::string& key) const;
    [[nodiscard]] std::string Get(const std::string& key, const std::string& default_value) const;
    [[nodiscard]] int Get(const std::string& key, int default_value) const;
    [[nodiscard]] float Get(const std::string& key, float default_value) const;
    void Set(const std::string& key, std::string value);
    void Set(const std::string& key, int value);
    void Set(const std::string& key, float value);
    [[nodiscard]] bool Has(const std::string& key) const;
    void Erase(const std::string& key);
    void Clear();

private:
    DataType data;
    mutable std::string m_serializeData;
};

} // namespace Common

class IParamPackageImpl;

class IParamPackageListImpl :
    public IParamPackageList
{
public:
    explicit IParamPackageListImpl(const std::vector<Common::ParamPackage>& list);
    ~IParamPackageListImpl();

    //IParamPackageList
    uint32_t GetCount() const override;
    IParamPackage& GetParamPackage(uint32_t index) const override;
    void Release() override;

private:
    IParamPackageListImpl() = delete;
    IParamPackageListImpl(const IParamPackageListImpl&) = delete;
    IParamPackageListImpl& operator=(const IParamPackageListImpl&) = delete;
    IParamPackageListImpl(IParamPackageListImpl&&) = delete;
    IParamPackageListImpl& operator=(IParamPackageListImpl&&) = delete;

    std::vector<IParamPackageImpl *> m_list;
};

class IParamPackageImpl :
    public IParamPackage
{
public:
    explicit IParamPackageImpl(const Common::ParamPackage & package);

    bool Has(const char * key) const override;
    bool GetBool(const char * key, bool default_value) const override;
    int32_t GetInt(const char * key, int32_t default_value) const override;
    float GetFloat(const char * key, float default_value) const override;
    const char * GetString(const char * key, const char * default_value) const override;
    void SetFloat(const char* key, float value) override;
    const char * Serialize() const override;
    void Release() override;

private:
    IParamPackageImpl() = delete;
    IParamPackageImpl(const IParamPackageImpl&) = delete;
    IParamPackageImpl& operator=(const IParamPackageImpl&) = delete;
    IParamPackageImpl(IParamPackageImpl&&) = delete;
    IParamPackageImpl& operator=(IParamPackageImpl&&) = delete;

    Common::ParamPackage m_package;
};
