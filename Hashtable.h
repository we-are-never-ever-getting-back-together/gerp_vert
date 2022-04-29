#include <mutex>
#include <vector>
#include <atomic>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <memory>

struct Location
{
  int line_idx;
  int line_infile;
  int file_idx;

  Location(int line_id, int line_file, int file)
  {
    line_idx = line_id;
    line_infile = line_file;
    file_idx = file;
  }
};

struct Entry
{
  std::string word;
  std::vector<Location> locations;

  Entry(std::string w, Location loc)
  {
    word = w;
    locations.push_back(loc);
  }
};

struct BaseEntry
{
  std::string identifier;
  std::vector<Entry> entries;
  mutable std::timed_mutex mutex;

  BaseEntry(std::string id, Entry entry)
  {
    identifier = id;
    entries.push_back(entry);
  }

  BaseEntry(BaseEntry const &other)
  {
    std::unique_lock<std::timed_mutex> lock_other(other.mutex);
    identifier = other.identifier;
    entries = other.entries;
  }

  BaseEntry &operator=(BaseEntry const &other)
  {
    if (&other != this)
    {
      std::unique_lock<std::timed_mutex> lock_this(mutex, std::defer_lock);
      std::unique_lock<std::timed_mutex> lock_other(other.mutex, std::defer_lock);

      // ensure no deadlock
      std::lock(lock_this, lock_other);

      identifier = other.identifier;
      entries = other.entries;
    }
    return *this;
  };
};

struct FileData
{
  std::vector<std::string> lines;
  std::string filename;
  mutable std::timed_mutex mutex;

  FileData(std::string name)
  {
    filename = name;
  }

  FileData(FileData const &other)
  {
    std::unique_lock<std::timed_mutex> lock_other(other.mutex);
    filename = other.filename;
  }

  FileData &operator=(FileData const &other)
  {
    if (&other != this)
    {
      std::unique_lock<std::timed_mutex> lock_this(mutex, std::defer_lock);
      std::unique_lock<std::timed_mutex> lock_other(other.mutex, std::defer_lock);

      // ensure no deadlock
      std::lock(lock_this, lock_other);

      filename = other.filename;
    }
    return *this;
  };
};

struct Files
{
  std::vector<FileData> data;
  mutable std::timed_mutex mutex;
};

struct HashTableEntry
{
  std::vector<BaseEntry> based;
  mutable std::timed_mutex mutex;
};

using HT = std::vector<HashTableEntry>;

class HashTable
{
private:
  const int INITIAL_SIZE = 100069;
  const double RESIZE = 2.5;
  const double RATIO = 0.75;

  Files files;
  int baseWordCount;
  int hashtableSize = INITIAL_SIZE;
  HT hashtable = HT(100069);

  void insert(std::string word, Location loc);
  void processLine(std::string line, int lineNum, int fileIdx);
  void resize();

  std::vector<Location> query(std::string word, bool insentive);
  void printLine(Location location, std::ostream &outfile);

public:
  HashTable(int filecount)
  {
    files.data.reserve(filecount);
  };
  void processFile(std::string fileName);
  void printQueryData(std::string word, std::ostream &outfile, bool insensitive);
};