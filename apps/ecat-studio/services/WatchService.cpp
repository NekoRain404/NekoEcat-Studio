#include "WatchService.h"
#include "EcatClient.h"

// WatchService.cpp — Live SDO watch window with add/remove/refresh operations
//
// Implementation notes:
//   - Maintains a list of WatchEntry items (position, index, subIndex, type)
//   - refreshAll() triggers upload for each entry via EcatClient
//   - Tracks previous values and change flags per entry

WatchService::WatchService(EcatClient* client, QObject* parent) : QObject(parent), client_(client) {}

void WatchService::addEntry(int position, const QString& index, const QString& subIndex, const QString& type) {
    WatchEntry entry;
    entry.position = position;
    entry.index = index;
    entry.subIndex = subIndex;
    entry.type = type;
    entries_.append(entry);
}

void WatchService::removeEntry(int position, const QString& index, const QString& subIndex) {
    for (int i = 0; i < entries_.size(); ++i) {
        if (entries_[i].position == position && entries_[i].index == index && entries_[i].subIndex == subIndex) {
            entries_.removeAt(i);
            return;
        }
    }
}

void WatchService::refreshAll() {
    int requested = entries_.size();
    int succeeded = 0;
    for (int i = 0; i < entries_.size(); ++i) {
        auto& e = entries_[i];
        e.previousValue = e.value;
        e.changed = false;
        client_->upload(e.position, e.index, e.subIndex);
        ++succeeded;
    }
    emit refreshComplete(requested, succeeded);
}

int WatchService::entryCount() const {
    return entries_.size();
}

const WatchEntry& WatchService::entryAt(int i) const {
    return entries_[i];
}
