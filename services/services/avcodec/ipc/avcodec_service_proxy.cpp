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

#include "avcodec_service_proxy.h"
#include "avcodec_listener_stub.h"
#include "avsharedmemory_ipc.h"
#include "media_errors.h"
#include "media_log.h"
#include "media_parcel.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "AVCodecServiceProxy"};
}

namespace OHOS {
namespace Media {
AVCodecServiceProxy::AVCodecServiceProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IStandardAVCodecService>(impl)
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances create", FAKE_POINTER(this));
}

AVCodecServiceProxy::~AVCodecServiceProxy()
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances destroy", FAKE_POINTER(this));
}

int32_t AVCodecServiceProxy::SetListenerObject(const sptr<IRemoteObject> &object)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    (void)data.WriteRemoteObject(object);
    int32_t ret = Remote()->SendRequest(SET_LISTENER_OBJ, data, reply, option);
    if (ret != MSERR_OK) {
        MEDIA_LOGE("Set listener obj failed, error: %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

int32_t AVCodecServiceProxy::InitParameter(AVCodecType type, bool isMimeType, const std::string &name)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInt32(static_cast<int32_t>(type));
    data.WriteBool(isMimeType);
    data.WriteString(name);
    int32_t ret = Remote()->SendRequest(INIT_PARAMETER, data, reply, option);
    if (ret != MSERR_OK) {
        MEDIA_LOGE("Init parameter failed, error: %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

int32_t AVCodecServiceProxy::Configure(const Format &format)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    (void)MediaParcel::Marshalling(data, format);
    int32_t ret = Remote()->SendRequest(CONFIGURE, data, reply, option);
    if (ret != MSERR_OK) {
        MEDIA_LOGE("Set listener obj failed, error: %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

int32_t AVCodecServiceProxy::Prepare()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(PREPARE, data, reply, option);
    if (ret != MSERR_OK) {
        MEDIA_LOGE("Prepare failed, error: %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

int32_t AVCodecServiceProxy::Start()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(START, data, reply, option);
    if (ret != MSERR_OK) {
        MEDIA_LOGE("Start failed, error: %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

int32_t AVCodecServiceProxy::Stop()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(STOP, data, reply, option);
    if (ret != MSERR_OK) {
        MEDIA_LOGE("Stop failed, error: %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

int32_t AVCodecServiceProxy::Flush()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(FLUSH, data, reply, option);
    if (ret != MSERR_OK) {
        MEDIA_LOGE("Flush failed, error: %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

int32_t AVCodecServiceProxy::Reset()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(RESET, data, reply, option);
    if (ret != MSERR_OK) {
        MEDIA_LOGE("Reset failed, error: %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

int32_t AVCodecServiceProxy::Release()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(RELEASE, data, reply, option);
    if (ret != MSERR_OK) {
        MEDIA_LOGE("Release failed, error: %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

sptr<OHOS::Surface> AVCodecServiceProxy::CreateInputSurface()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(CREATE_INPUT_SURFACE, data, reply, option);
    if (ret != MSERR_OK) {
        MEDIA_LOGE("CreateInputSurface failed, error: %{public}d", ret);
        return nullptr;
    }

    sptr<IRemoteObject> object = reply.ReadRemoteObject();
    if (object == nullptr) {
        MEDIA_LOGE("failed to read surface object");
        return nullptr;
    }

    sptr<IBufferProducer> producer = iface_cast<IBufferProducer>(object);
    if (producer == nullptr) {
        MEDIA_LOGE("failed to convert object to producer");
        return nullptr;
    }

    return OHOS::Surface::CreateSurfaceAsProducer(producer);
}

int32_t AVCodecServiceProxy::SetOutputSurface(sptr<OHOS::Surface> surface)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    CHECK_AND_RETURN_RET_LOG(surface != nullptr, MSERR_NO_MEMORY, "surface is nullptr");
    sptr<IBufferProducer> producer = surface->GetProducer();
    CHECK_AND_RETURN_RET_LOG(producer != nullptr, MSERR_NO_MEMORY, "producer is nullptr");

    sptr<IRemoteObject> object = producer->AsObject();
    CHECK_AND_RETURN_RET_LOG(object != nullptr, MSERR_NO_MEMORY, "object is nullptr");

    const std::string surfaceFormat = "SURFACE_FORMAT";
    std::string format = surface->GetUserData(surfaceFormat);
    MEDIA_LOGI("surfaceFormat is %{public}s!", format.c_str());
    (void)data.WriteRemoteObject(object);
    data.WriteString(format);
    int32_t error = Remote()->SendRequest(SET_OUTPUT_SURFACE, data, reply, option);
    if (error != MSERR_OK) {
        MEDIA_LOGE("SetOutputSurface failed, error: %{public}d", error);
        return error;
    }
    return reply.ReadInt32();
}

std::shared_ptr<AVSharedMemory> AVCodecServiceProxy::GetInputBuffer(uint32_t index)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteUint32(index);
    int32_t ret = Remote()->SendRequest(GET_INPUT_BUFFER, data, reply, option);
    if (ret != MSERR_OK) {
        MEDIA_LOGE("GetInputBuffer failed, error: %{public}d", ret);
        return nullptr;
    }
    auto memory = ReadAVSharedMemoryFromParcel(reply);
    if (memory == nullptr) {
        MEDIA_LOGE("Failed to GetInputBuffer");
        return nullptr;
    }
    return memory;
}

int32_t AVCodecServiceProxy::QueueInputBuffer(uint32_t index, AVCodecBufferInfo info, AVCodecBufferFlag flag)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteUint32(index);
    data.WriteInt64(info.presentationTimeUs);
    data.WriteInt32(info.size);
    data.WriteInt32(info.offset);
    data.WriteInt32(static_cast<int32_t>(flag));
    int32_t ret = Remote()->SendRequest(QUEUE_INPUT_BUFFER, data, reply, option);
    if (ret != MSERR_OK) {
        MEDIA_LOGE("QueueInputBuffer failed, error: %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

std::shared_ptr<AVSharedMemory> AVCodecServiceProxy::GetOutputBuffer(uint32_t index)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteUint32(index);
    int32_t ret = Remote()->SendRequest(GET_OUTPUT_BUFFER, data, reply, option);
    if (ret != MSERR_OK) {
        MEDIA_LOGE("GetOutputBuffer failed, error: %{public}d", ret);
        return nullptr;
    }
    auto memory = ReadAVSharedMemoryFromParcel(reply);
    if (memory == nullptr) {
        MEDIA_LOGE("Failed to GetOutputBuffer");
        return nullptr;
    }
    return memory;
}

int32_t AVCodecServiceProxy::GetOutputFormat(Format &format)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(GET_OUTPUT_FORMAT, data, reply, option);
    if (ret != MSERR_OK) {
        MEDIA_LOGE("GetOutputFormat failed, error: %{public}d", ret);
        return ret;
    }

    (void)MediaParcel::Unmarshalling(reply, format);
    return MSERR_OK;
}

int32_t AVCodecServiceProxy::ReleaseOutputBuffer(uint32_t index, bool render)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteUint32(index);
    data.WriteBool(render);
    int32_t ret = Remote()->SendRequest(RELEASE_OUTPUT_BUFFER, data, reply, option);
    if (ret != MSERR_OK) {
        MEDIA_LOGE("ReleaseOutputBuffer failed, error: %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

int32_t AVCodecServiceProxy::SetParameter(const Format &format)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    (void)MediaParcel::Marshalling(data, format);
    int32_t ret = Remote()->SendRequest(SET_PARAMETER, data, reply, option);
    if (ret != MSERR_OK) {
        MEDIA_LOGE("SetParameter failed, error: %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

int32_t AVCodecServiceProxy::DestroyStub()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(DESTROY, data, reply, option);
    if (ret != MSERR_OK) {
        MEDIA_LOGE("destroy failed, error: %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}
} // namespace Media
} // namespace OHOS
