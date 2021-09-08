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

#ifndef GST_HDI_VIDEO_H
#define GST_HDI_VIDEO_H

#include "gst_hdi.h"

GstVideoFormat gst_hdi_video_pixelformt_to_gstvideoformat(PixelFormat hdiColorformat);
void gst_hdi_video_set_caps_pixelformat(GstCaps *caps, const GList *formats);
#endif /* GST_HDI_VIDEO_H */