/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License\n");
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

#include "player_demo.h"
#include <iostream>
#include "media_data_source_demo_noseek.h"
#include "media_data_source_demo_seekable.h"
#include "string_ex.h"
#include "media_errors.h"
#include "directory_ex.h"

using namespace OHOS;
using namespace OHOS::Media;
using namespace MediaDemo;
using namespace std;
namespace {
const std::string SURFACE_STRIDE_ALIGNMENT = "SURFACE_STRIDE_ALIGNMENT";
const std::string SURFACE_FORMAT = "SURFACE_FORMAT";
const float EPSINON = 0.0001;
const float SPEED_0_75_X = 0.75;
const float SPEED_1_00_X = 1.00;
const float SPEED_1_25_X = 1.25;
const float SPEED_1_75_X = 1.75;
const float SPEED_2_00_X = 2.00;
}

// PlayerCallback override
void PlayerCallbackDemo::OnError(PlayerErrorType errorType, int32_t errorCode)
{
    (void)errorType;
    cout << "Error received, errorCode:" << MSErrorToString(static_cast<MediaServiceErrCode>(errorCode)) << endl;
}

void PlayerCallbackDemo::OnInfo(PlayerOnInfoType type, int32_t extra, const Format &infoBody)
{
    (void)infoBody;
    switch (type) {
        case INFO_TYPE_SEEKDONE:
            cout << "PlayerCallback: OnSeekDone currentPositon is " << extra << endl;
            break;
        case INFO_TYPE_EOS:
            cout << "PlayerCallback: OnEndOfStream isLooping is " << extra << endl;
            break;
        case INFO_TYPE_STATE_CHANGE:
            state_ = static_cast<PlayerStates>(extra);
            PrintState(state_);
            break;
        case INFO_TYPE_POSITION_UPDATE:
            if (updateCount_ == POSITION_UPDATE_INTERVAL) {
                cout << "OnPositionUpdated positon is " << extra << endl;
                updateCount_ = 0;
            }
            updateCount_++;
            break;
        case INFO_TYPE_MESSAGE:
            cout << "PlayerCallback: OnMessage is " << extra << endl;
            break;
        default:
            break;
    }
}

void PlayerCallbackDemo::PrintState(PlayerStates state) const
{
    if (STATE_MAP.count(state) != 0) {
        cout << "State:" << (STATE_MAP.at(state)).c_str() << endl;
    } else {
        cout << "Invalid state" << endl;
    }
}

sptr<Surface> PlayerDemo::GetVideoSurface()
{
    cout << "Please enter the number of mode(default no window):" << endl;
    cout << "0:no window" << endl;
    cout << "1:window" << endl;
    cout << "2:sub window" << endl;
    string mode;
    (void)getline(cin, mode);
    sptr<Surface> producerSurface = nullptr;
    if (mode == "0" || mode == "") {
        return nullptr;
    } else if (mode == "1") {
        sptr<WindowManager> wmi = WindowManager::GetInstance();
        if (wmi == nullptr) {
            cout << "WindowManager is null" << endl;
            return nullptr;
        }
        (void)wmi->Init();
        sptr<WindowOption> option = WindowOption::Get();
        if (option == nullptr) {
            cout << "WindowOption is null" << endl;
            return nullptr;
        }
        (void)option->SetWidth(640);
        (void)option->SetHeight(360);
        (void)option->SetX(0);
        (void)option->SetY(0);
        (void)option->SetWindowType(WINDOW_TYPE_NORMAL);
        (void)wmi->CreateWindow(mwindow_, option);
        if (mwindow_ == nullptr) {
            cout << "mwindow_ is null" << endl;
            return nullptr;
        }
        producerSurface = mwindow_->GetSurface();
    } else if (mode == "2") {
        cout << "invalid operation" << endl;
        return nullptr;
    }
    if (producerSurface == nullptr) {
        cout << "producerSurface is nullptr" << endl;
        return nullptr;
    }
    (void)producerSurface->SetUserData(SURFACE_FORMAT, std::to_string(static_cast<int>(PIXEL_FMT_RGBA_8888)));
    cout << "GetVideoSurface ok" << endl;
    return producerSurface;
}

void PlayerDemo::Seek(const std::string cmd)
{
    int32_t time = -1;
    if (!StrToInt(cmd, time) || time < 0) {
        cout << "You need to configure the seek time parameter" << endl;
        return;
    }
    if (player_->Seek(time, PlayerSeekMode::SEEK_PREVIOUS_SYNC) != 0) {
        cout << "Operation Failed" << endl;
    } else {
        cout << "Operation OK" << endl;
    }
}

int32_t PlayerDemo::ChangeModeToSpeed(const PlaybackRateMode &mode, double &rate) const
{
    if (mode == SPEED_FORWARD_0_75_X) {
        rate = SPEED_0_75_X;
    } else if (mode == SPEED_FORWARD_1_00_X) {
        rate = SPEED_1_00_X;
    } else if (mode == SPEED_FORWARD_1_25_X) {
        rate = SPEED_1_25_X;
    } else if (mode == SPEED_FORWARD_1_75_X) {
        rate = SPEED_1_75_X;
    } else if (mode == SPEED_FORWARD_2_00_X) {
        rate = SPEED_2_00_X;
    } else {
        return -1;
    }
    return 0;
}

int32_t PlayerDemo::ChangeSpeedToMode(const double &rate, PlaybackRateMode &mode) const
{
    if (abs(rate - SPEED_0_75_X) < EPSINON) {
        mode = SPEED_FORWARD_0_75_X;
    } else if (abs(rate - SPEED_1_00_X) < EPSINON) {
        mode = SPEED_FORWARD_1_00_X;
    } else if (abs(rate - SPEED_1_25_X) < EPSINON) {
        mode = SPEED_FORWARD_1_25_X;
    } else if (abs(rate - SPEED_1_75_X) < EPSINON) {
        mode = SPEED_FORWARD_1_75_X;
    } else if (abs(rate - SPEED_2_00_X) < EPSINON) {
        mode = SPEED_FORWARD_2_00_X;
    } else {
        return -1;
    }
    return  0;
}

int32_t PlayerDemo::GetPlaybackSpeed() const
{
    PlaybackRateMode mode;
    double rate;
    (void)player_->GetPlaybackSpeed(mode);
    if (ChangeModeToSpeed(mode, rate) != 0) {
        cout << "Change mode " << mode << " to speed failed" << endl;
    } else {
        cout << "Speed:" << rate << endl;
    }
    return 0;
}

void PlayerDemo::SetPlaybackSpeed(const std::string cmd) const
{
    PlaybackRateMode mode;
    if (!cmd.empty()) {
        double rate = std::stod(cmd.c_str());
        if (ChangeSpeedToMode(rate, mode) != 0) {
            cout << "Change speed " << rate << " to mode failed" << endl;
            return;
        }

        if (player_->SetPlaybackSpeed(mode) != 0) {
            cout << "Operation Failed" << endl;
        } else {
            cout << "Operation OK" << endl;
        }
    }
}

void PlayerDemo::SetLoop(const std::string cmd)
{
    int32_t loopEn = -1;
    if (!StrToInt(cmd, loopEn)) {
        cout << "You need to configure the loop parameter" << endl;
        return;
    }
    if (player_->SetLooping(static_cast<bool>(loopEn)) != 0) {
        cout << "Operation Failed" << endl;
    } else {
        cout << "Operation OK" << endl;
    }
}

int32_t PlayerDemo::GetPlaying()
{
    bool isPlay = player_->IsPlaying();
    cout << "Playing:" << isPlay << endl;
    return 0;
}

int32_t PlayerDemo::GetLooping()
{
    bool isLoop = player_->IsLooping();
    cout << "Looping:" << isLoop << endl;
    return 0;
}

void PlayerDemo::DoNext()
{
    cout << "Enter your step:" << endl;
    std::string cmd;
    while (std::getline(std::cin, cmd)) {
        auto iter = playerTable_.find(cmd);
        if (iter != playerTable_.end()) {
            auto func = iter->second;
            if (func() != 0) {
                cout << "Operation error" << endl;
            }
            continue;
        } else if (cmd.find("source") != std::string::npos) {
            (void)SelectSource(cmd.substr(cmd.find_last_of("source ") + 1));
            continue;
        } else if (cmd.find("seek") != std::string::npos) {
            Seek(cmd.substr(cmd.find_last_of("seek ") + 1));
            continue;
        } else if (cmd.find("volume") != std::string::npos) {
            std::string volume = cmd.substr(cmd.find_last_of("volume ") + 1);
            if (!volume.empty()) {
                (void)player_->SetVolume(std::stof(volume.c_str()), std::stof(volume.c_str()));
            }
            continue;
        } else if (cmd.find("duration") != std::string::npos) {
            int32_t duration = -1;
            (void)player_->GetDuration(duration);
            cout << "GetDuration:" << duration << endl;
            continue;
        } else if (cmd.find("time") != std::string::npos) {
            int32_t time = -1;
            (void)player_->GetCurrentTime(time);
            cout << "GetCurrentTime:" << time << endl;
            continue;
        } else if (cmd.find("loop") != std::string::npos) {
            SetLoop(cmd.substr(cmd.find_last_of("loop ") + 1));
            continue;
        } else if (cmd.find("speed") != std::string::npos) {
            SetPlaybackSpeed(cmd.substr(cmd.find_last_of("speed ") + 1));
            continue;
        } else if (cmd.find("quit") != std::string::npos ||
            cmd == "q") {
            break;
        }
    }
}

void PlayerDemo::RegisterTable()
{
    (void)playerTable_.emplace("prepare", std::bind(&Player::Prepare, player_));
    (void)playerTable_.emplace("prepareasync", std::bind(&Player::PrepareAsync, player_));
    (void)playerTable_.emplace("", std::bind(&Player::Play, player_)); // ENTER -> play
    (void)playerTable_.emplace("play", std::bind(&Player::Play, player_));
    (void)playerTable_.emplace("pause", std::bind(&Player::Pause, player_));
    (void)playerTable_.emplace("stop", std::bind(&Player::Stop, player_));
    (void)playerTable_.emplace("reset", std::bind(&Player::Reset, player_));
    (void)playerTable_.emplace("release", std::bind(&Player::Release, player_));
    (void)playerTable_.emplace("isplaying", std::bind(&PlayerDemo::GetPlaying, this));
    (void)playerTable_.emplace("isloop", std::bind(&PlayerDemo::GetLooping, this));
    (void)playerTable_.emplace("speed", std::bind(&PlayerDemo::GetPlaybackSpeed, this));
}

int32_t PlayerDemo::SetDataSrc(const string &path, bool seekable)
{
    std::shared_ptr<IMediaDataSource> dataSrc = nullptr;
    cout << "Please enter the size of buffer:" << endl;
    cout << "0:default" << endl;
    string sizeStr;
    (void)getline(cin, sizeStr);
    int32_t size = -1;
    if (!StrToInt(sizeStr, size) || size < 0) {
        cout << "default size" << endl;
    } else {
        cout << "buffer size:" << size << endl;
    }
    if (seekable == true) {
        dataSrc = MediaDataSourceDemoSeekable::Create(path, size);
    } else {
        dataSrc = MediaDataSourceDemoNoSeek::Create(path, size);
    }
    return player_->SetSource(dataSrc);
}

int32_t PlayerDemo::SelectSource(const string &pathOuter)
{
    string path;
    int32_t ret = -1;
    if (pathOuter == "") {
        cout << "Please enter the video/audio path: " << endl;
        (void)getline(cin, path);
    } else {
        path = pathOuter;
    }
    std::string realPath;
    if (!PathToRealPath(path, realPath)) {
        cout << "Path is unaccessable: " << path << endl;
        return -1;
    }
    cout << "Path is " << path << endl;
    cout << "Please enter the number of source mode(default LOCAL):" << endl;
    cout << "0:local file source" << endl;
    cout << "1:stream file source with no seek" << endl;
    cout << "2:stream file source with seekable" << endl;
    string srcMode;
    (void)getline(cin, srcMode);
    if (srcMode == "" || srcMode == "0") {
        cout << "source mode is LOCAL" << endl;
        ret = player_->SetSource(realPath);
    } else if (srcMode == "1") {
        cout << "source mode is stream NO seek" << endl;
        ret = SetDataSrc(realPath, false);
    } else if (srcMode == "2") {
        cout << "source mode is stream seekable" << endl;
        ret = SetDataSrc(realPath, true);
    } else {
        cout << "unknow mode" << endl;
    }
    return ret;
}

void PlayerDemo::RunCase(const string &path)
{
    player_ = OHOS::Media::PlayerFactory::CreatePlayer();
    if (player_ == nullptr) {
        cout << "player_ is null" << endl;
        return;
    }
    RegisterTable();
    std::shared_ptr<PlayerCallbackDemo> cb = std::make_shared<PlayerCallbackDemo>();
    int32_t ret = player_->SetPlayerCallback(cb);
    if (ret != 0) {
        cout << "SetPlayerCallback fail" << endl;
    }
    if (SelectSource(path) != 0) {
        cout << "SetSource fail" << endl;
        return;
    }
    sptr<Surface> producerSurface = nullptr;
    producerSurface = GetVideoSurface();
    if (producerSurface != nullptr) {
        ret = player_->SetVideoSurface(producerSurface);
        if (ret != 0) {
            cout << "SetVideoSurface fail" << endl;
        }
    }

    ret = player_->PrepareAsync();
    if (ret !=  0) {
        cout << "PrepareAsync fail" << endl;
        return;
    }
    DoNext();
}