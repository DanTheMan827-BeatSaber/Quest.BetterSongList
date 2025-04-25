#include "Patches/HookSelectedInTable.hpp"
#include "config.hpp"
#include "logging.hpp"

namespace BetterSongList::Hooks {
    void HookSelectedInTable::LevelCollectionTableView_HandleDidSelectCellWithIndex_Postfix(GlobalNamespace::BeatmapLevel* selectedPreviewBeatmapLevel) {
        config.set_lastSong(selectedPreviewBeatmapLevel ? static_cast<std::string>(selectedPreviewBeatmapLevel->levelID) : "");
        INFO("LevelCollectionTableView.HandleDidSelectCellWithIndex(): LastSong: {}", config.get_lastSong());
    }
}
