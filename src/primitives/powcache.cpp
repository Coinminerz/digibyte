// Copyright (c) 2022-2023 The Raptoreum developers
// Copyright (c) 2023 The DigiByte Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// Copyright (c) 2022 The Raptoreum developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <primitives/powcache.h>
#include <fs.h>
#include <primitives/block.h>
#include <streams.h>
#include <hash.h>
#include <sync.h>
#include <util/system.h>

RecursiveMutex cs_pow;

CPowCache* CPowCache::instance = nullptr;

CPowCache& CPowCache::Instance()
{
    if (CPowCache::instance == nullptr) {
        CPowCache::instance = new CPowCache();
    }
    return *CPowCache::instance;
}

CPowCache::CPowCache()
    : unordered_lru_cache<uint256, uint256, std::hash<uint256>>(DEFAULT_POWCACHE_MAX_ELEMENTS)
    , nVersion(CURRENT_VERSION)
    , nSavedSize(0)
    , bValidate(DEFAULT_POWCACHE_VALIDATE)
    , nSaveInterval(DEFAULT_POWCACHE_SAVE_INTERVAL)
{
}

CPowCache::~CPowCache()
{
}

void CPowCache::DoMaintenance() {

    LOCK(cs_pow);

    int64_t n_start = GetTimeMillis();

    // If cache has grown enough, save it:
    if (WantsToSave()) {
        CAutoFile file(fsbridge::fopen(gArgs.GetDataDirNet() / "powcache.dat", "wb"), SER_DISK, POWCACHE_CURRENT_VERSION);
        if (file.IsNull()) {
            LogPrintf("%s: Unable to save to file\n", this->instance->ToString());
        }
        else
        {
            this->instance->Serialize(file);
            LogPrint(BCLog::POWCACHE, "Flushed %d powcache elements to disk  %dms\n", size(),
                    GetTimeMillis() - n_start);
        }

    }
}

void CPowCache::SetMaxElements(int64_t maxElements)
{
    if (maxElements > 0) {
        maxSize = maxElements;
    }
}

void CPowCache::SetValidate(bool validate)
{
    bValidate = validate;
}

void CPowCache::SetSaveInterval(int saveInterval)
{
    nSaveInterval = saveInterval;
}

bool CPowCache::WantsToSave() const
{
   return size() - nSavedSize >= nSaveInterval;
}

std::string CPowCache::ToString() const
{
    std::ostringstream info;
    info << "PowCache: elements: " << size();
    return info.str();
}