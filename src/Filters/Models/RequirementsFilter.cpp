#include "Filters/Models/RequirementsFilter.hpp"

#include "songloader/shared/API.hpp"
#include "songloader/shared/CustomTypes/CustomLevelInfoSaveData.hpp"
#include "songloader/include/CustomTypes/SongLoader.hpp"

#include "System/Threading/Tasks/Task_1.hpp"
#include "Utils/BeatmapUtils.hpp"

namespace BetterSongList {
    SafePtr<System::Threading::Tasks::TaskCompletionSource_1<bool>> RequirementsFilter::loadingTask;
    bool RequirementsFilter::inited = false;

    RequirementsFilter::RequirementsFilter() 
        : IFilter() {}

    bool RequirementsFilter::get_isReady() const {
        return RuntimeSongLoader::SongLoader::GetInstance()->HasLoaded;
    }

    void RequirementsFilter::SongsLoadedCallback(std::vector<GlobalNamespace::CustomPreviewBeatmapLevel*> const& songs) {
        /* don't quite understand what to do here tbh */
        loadingTask->SetResult(true);
    }

    System::Threading::Tasks::Task* RequirementsFilter::Prepare() {
        if (!loadingTask || loadingTask->get_Task()->get_IsCompleted()) {
            loadingTask = System::Threading::Tasks::TaskCompletionSource_1<bool>::New_ctor();
        }
        if (!inited && (inited = true)) {
            RuntimeSongLoader::API::AddSongsLoadedEvent(std::bind(&RequirementsFilter::SongsLoadedCallback, this, std::placeholders::_1));
        }

        return reinterpret_cast<System::Threading::Tasks::Task*>(loadingTask->get_Task());
    }

    bool RequirementsFilter::GetValueFor(GlobalNamespace::IPreviewBeatmapLevel* level) {
        auto h = BeatmapUtils::GetHashOfPreview(level);
        if (h.empty()) return false;

        auto customLevel = il2cpp_utils::try_cast<GlobalNamespace::CustomPreviewBeatmapLevel>(level).value_or(nullptr);
        if (!customLevel) return false;
        auto saveData = customLevel->get_standardLevelInfoSaveData();
        if (!saveData) return false;
        auto customSaveData = il2cpp_utils::try_cast<CustomJSONData::CustomLevelInfoSaveData>(saveData).value_or(nullptr);
        if (!customSaveData || !customSaveData->customData.has_value()) return false;

        auto& customData = customSaveData->customData.value().get();

        auto requirementsItr = customData.FindMember(u"_requirements");
        if (requirementsItr == customData.MemberEnd()) return false;
        return requirementsItr->value.Size() > 0;
    }
}