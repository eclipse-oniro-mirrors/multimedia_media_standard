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

#ifndef I_STANDARD_MEDIA_SERVICE_H
#define I_STANDARD_MEDIA_SERVICE_H

#include "ipc_types.h"
#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Media {
class IStandardMediaService : public IRemoteBroker {
public:
    /**
     * sub system ability ID
     */
    enum MediaSystemAbility {
        MEDIA_PLAYER = 0,
        MEDIA_RECORDER = 1,
        MEDIA_CODEC = 2,
        MEDIA_AVMETADATAHELPER = 3,
        MEDIA_CODECLIST = 4,
        MEDIA_AVCODEC = 5,
    };

    /**
     * Create Player/Recorder Service Ability
     *
     * @return Returns remote object sptr, nullptr on failure.
     */
    virtual sptr<IRemoteObject> GetSubSystemAbility(IStandardMediaService::MediaSystemAbility subSystemId) = 0;
    virtual int32_t SetListenerObject(const sptr<IRemoteObject> &object) = 0;
    /**
     * IPC code ID
     */
    enum MediaServiceMsg {
        GET_SUBSYSTEM = 0,
        SET_LISTENER_OBJ,
    };

    DECLARE_INTERFACE_DESCRIPTOR(u"IStandardMediaService");
};
} // namespace Media
} // namespace OHOS
#endif // I_STANDARD_MEDIA_SERVICE_H
