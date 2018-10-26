#include "../core/FileWatcher.h"

namespace gplay {


FileWatcherBase::FileWatcherBase()
{
    // create instance of FileWatcher and start watching.
    _fileWatcher = new efsw::FileWatcher();
    _fileWatcher->watch();
}

FileWatcherBase::~FileWatcherBase()
{
    SAFE_DELETE(_fileWatcher);
}

void FileWatcherBase::addDirectory(const char* directory, bool recursive)
{
    GP_ASSERT(_fileWatcher);

    // add a watch directory task
    efsw::WatchID watchID = _fileWatcher->addWatch(directory, this, recursive);
}

void FileWatcherBase::handleFileAction( efsw::WatchID watchid,
                                        const std::string& dir,
                                        const std::string& filename,
                                        efsw::Action action, std::string oldFilename)
{
    switch( action )
    {
    case efsw::Actions::Add:
        print("FileWatcher: %s%s has been added.\n", dir.c_str(), filename.c_str());
        break;
    case efsw::Actions::Delete:
        print("FileWatcher: %s%s has been deleted.\n", dir.c_str(), filename.c_str());
        break;
    case efsw::Actions::Modified:
        print("FileWatcher: %s%s has been modified.\n", dir.c_str(), filename.c_str());
        break;
    case efsw::Actions::Moved:
        print("FileWatcher: %s%s has been moved from %s.\n", dir.c_str(), filename.c_str(), oldFilename.c_str());
        break;
    default:
        print("FileWatcher: unknow action.");
    }

    // send event
    FileWatcherEvent::WatchData args;
    args.action = action;
    args.directory = dir;
    args.filename = filename;
    args.oldFilename = oldFilename;
    EventManager::getInstance()->queueEvent(FileWatcherEvent::create(args));
}



}
