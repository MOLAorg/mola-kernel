/* -------------------------------------------------------------------------
 *   A Modular Optimization framework for Localization and mApping  (MOLA)
 * Copyright (C) 2018-2019 Jose Luis Blanco, University of Almeria
 * See LICENSE for license information.
 * ------------------------------------------------------------------------- */
/**
 * @file   FrontEndBase.cpp
 * @brief  Virtual interface for SLAM front-ends
 * @author Jose Luis Blanco Claraco
 * @date   Dec 11, 2018
 */

#include <mola-kernel/FrontEndBase.h>
#include <mola-kernel/RawDataSourceBase.h>
#include <yaml-cpp/yaml.h>
#include <iostream>

using namespace mola;

FrontEndBase::FrontEndBase() = default;

void FrontEndBase::initialize_common(const std::string& cfg_block)
{
    MRPT_TRY_START
    auto cfg = YAML::Load(cfg_block);

    // To-do: think: Really need to limit to only one sensor per front-end?
    auto ds_sensor_label = cfg["raw_sensor_label"];
    ASSERTMSG_(ds_sensor_label, "Missing parameter `raw_sensor_label`");
    raw_sensor_label_ = ds_sensor_label.as<std::string>();

    auto ds_source = cfg["raw_data_source"];
    ASSERTMSG_(ds_source, "Missing parameter `raw_data_source`");
    const auto src_name = ds_source.as<std::string>();

    ASSERT_(this->nameServer_);

    auto data_src = nameServer_(src_name);
    if (!data_src)
        THROW_EXCEPTION_FMT(
            "Cannot find data source module named `%s`", src_name.c_str());

    auto rdsb = std::dynamic_pointer_cast<RawDataSourceBase>(data_src);
    if (!rdsb)
        THROW_EXCEPTION_FMT(
            "Could not cast data source module named `%s` to RawDataSourceBase",
            src_name.c_str());

    // Subscribe:
    rdsb->attachToDataConsumer(*this);

    MRPT_TRY_END
}
