/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "avcodec_ability_singleton.h"
#include "avcodec_xml_parser.h"
#include "codec_plugins_capability.h"
#include "media_log.h"
#include "media_errors.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "AVCodecAbilitySingleton"};
}

namespace OHOS {
namespace Media {
AVCodecAbilitySingleton& AVCodecAbilitySingleton::GetInstance()
{
    static AVCodecAbilitySingleton instance;
    return instance;
}

AVCodecAbilitySingleton::AVCodecAbilitySingleton()
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances create", FAKE_POINTER(this));
}

AVCodecAbilitySingleton::~AVCodecAbilitySingleton()
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances destroy", FAKE_POINTER(this));
}

bool AVCodecAbilitySingleton::ParseCodecXml()
{
    AVCodecXmlParser xmlParser;
    bool ret = xmlParser.LoadConfiguration();
    if (!ret) {
        this->isParsered_ = false;
        MEDIA_LOGE("AVCodecList LoadConfiguration failed");
        return false;
    }
    ret = xmlParser.Parse();
    if (!ret) {
        isParsered_ = false;
        MEDIA_LOGE("AVCodecList Parse failed.");
        return false;
    }
    std::vector<CapabilityData> data = xmlParser.GetCapabilityDataArray();
    capabilityDataArray_.insert(capabilityDataArray_.end(), data.begin(), data.end());
    isParsered_ = true;
    return true;
}

bool AVCodecAbilitySingleton::ParseHardwareCapability()
{
    MEDIA_LOGD("ParseHardwareCapability start");
    std::vector<CapabilityData> data = CodecPluginsCapability::GetInstance().GetCodecPluginsCapability();
    capabilityDataArray_.insert(capabilityDataArray_.end(), data.begin(), data.end());
    MEDIA_LOGD("ParseHardwareCapability end");
    return true;
}

bool AVCodecAbilitySingleton::IsParsered() const
{
    return isParsered_;
}

std::vector<CapabilityData> AVCodecAbilitySingleton::GetCapabilityDataArray() const
{
    return capabilityDataArray_;
}
} // namespace Media
} // namespace OHOS
