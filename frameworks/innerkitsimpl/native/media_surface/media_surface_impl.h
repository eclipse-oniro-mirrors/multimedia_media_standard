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

#ifndef MEDIA_SURFACE_IMPL_H
#define MEDIA_SURFACE_IMPL_H

#include <map>
#include <string>
#include "media_surface.h"
#include "window_manager.h"
#include "foundation/windowmanager/interfaces/innerkits/wm/window.h"

namespace OHOS {
namespace Media {
class MediaSurfaceImpl : public MediaSurface, public NoCopyable {
public:
    MediaSurfaceImpl();
    ~MediaSurfaceImpl();

    std::string GetSurfaceId(const sptr<Surface> &surface) override;
    sptr<Surface> GetSurface() override;
    sptr<Surface> GetSurface(const std::string &id) override;
    void Release() override;

private:
    std::mutex mutex_;
    int32_t idCount = 0;
    int32_t id_ = 0;
    sptr<Window> mwindow_ = nullptr;
    sptr<Rosen::Window> previewWindow_ = nullptr;
    std::map<std::string, wptr<Surface>> surfaceMap_;
    sptr<Surface> producerSurface_ = nullptr;
};
} // namespace Media
} // namespace OHOS
#endif // MEDIA_SURFACE_IMPL_H